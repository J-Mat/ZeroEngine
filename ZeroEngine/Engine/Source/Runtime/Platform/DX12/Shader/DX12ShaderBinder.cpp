#include "DX12ShaderBinder.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Platform/DX12/MemoryManage/DynamicDescriptorHeap.h"
#include "../DX12CommandList.h"

namespace Zero
{

#define COPY_DATA_TO_CONSTBUFFER(Name, Value)  \
		do{\
			FShaderConstantItem Item; \
			if (m_ConstantsMapper.FetchConstant(Name, Item))\
			{\
				m_ConstantsTableBuffer->CopyDataToConstantsBuffer(TO_VOIDPTR_FROM_DATA(Value), Item.Offset, ShaderDataTypeSize(Item.Type)); \
				m_bIsDirty = true; \
			}\
		}while(0)

#define COPY_DATA_FROM_CONSTBUFFER(Name, Value)  \
		do{\
			FShaderConstantItem Item; \
			if (m_ConstantsMapper.FetchConstant(Name, Item))\
			{\
				m_ConstantsTableBuffer->CopyDataFromConstantsBuffer(TO_VOIDPTR_FROM_DATA(Value), Item.Offset, ShaderDataTypeSize(Item.Type)); \
			}\
		}while(0)

#define COPY_PTR_FROM_CONSTBUFFER(Name, Value, Type)  \
			FShaderConstantItem Item; \
			if (m_ConstantsMapper.FetchConstant(Name, Item))\
			{\
				return (Type*)m_ConstantsTableBuffer->GetPtrFromConstantsBuffer(Item.Offset); \
			}\
			return nullptr;

	FDX12ShaderConstantsBuffer::FDX12ShaderConstantsBuffer(FShaderConstantsDesc& Desc)
		: IShaderConstantsBuffer(Desc)
		, m_ConstantsMapper(Desc.Mapper)
	{
		m_ConstantsTableBuffer = CreateRef<FFrameResourceBuffer>(m_Desc.Size);
	}

	void FDX12ShaderConstantsBuffer::SetInt(const std::string& Name, const int& Value)
	{
		COPY_DATA_TO_CONSTBUFFER(Name, Value);
	}

	void FDX12ShaderConstantsBuffer::SetFloat(const std::string& Name, const float& Value)
	{
		COPY_DATA_TO_CONSTBUFFER(Name, Value);
	}

	void FDX12ShaderConstantsBuffer::SetFloat2(const std::string& Name, const ZMath::vec2& Value)
	{
		COPY_DATA_TO_CONSTBUFFER(Name, Value[0]);
	}

	void FDX12ShaderConstantsBuffer::SetFloat3(const std::string& Name, const ZMath::vec3& Value)
	{
		COPY_DATA_TO_CONSTBUFFER(Name, Value[0]);
	}

	void FDX12ShaderConstantsBuffer::SetFloat4(const std::string& Name, const ZMath::vec4& Value)
	{
		COPY_DATA_TO_CONSTBUFFER(Name, Value[0]);
	}

	void FDX12ShaderConstantsBuffer::SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value)
	{
		COPY_DATA_TO_CONSTBUFFER(Name, Value[0][0]);
	}

	void FDX12ShaderConstantsBuffer::GetInt(const std::string& Name, int& Value)
	{
		COPY_DATA_FROM_CONSTBUFFER(Name, Value);
	}

	void FDX12ShaderConstantsBuffer::GetFloat(const std::string& Name, float& Value)
	{
		COPY_DATA_FROM_CONSTBUFFER(Name, Value);
	}

	void FDX12ShaderConstantsBuffer::GetFloat3(const std::string& Name, ZMath::vec3& Value)
	{
		COPY_DATA_FROM_CONSTBUFFER(Name, Value[0]);
	}

	void FDX12ShaderConstantsBuffer::GetFloat4(const std::string& Name, ZMath::vec4& Value)
	{
		COPY_DATA_FROM_CONSTBUFFER(Name, Value[0]);
	}

	void FDX12ShaderConstantsBuffer::GetMatrix4x4(const std::string& Name, ZMath::mat4& Value)
	{
		COPY_DATA_FROM_CONSTBUFFER(Name, Value[0][0]);
	}

	float* FDX12ShaderConstantsBuffer::PtrFloat(const std::string& Name)
	{
		COPY_PTR_FROM_CONSTBUFFER(Name, Value, float)
	}

	float* FDX12ShaderConstantsBuffer::PtrFloat3(const std::string& Name)
	{
		COPY_PTR_FROM_CONSTBUFFER(Name, Value, float)
	}

	float* FDX12ShaderConstantsBuffer::PtrFloat4(const std::string& Name)
	{
		COPY_PTR_FROM_CONSTBUFFER(Name, Value, float)
	}

	float* FDX12ShaderConstantsBuffer::PtrMatrix4x4(const std::string& Name)
	{
		COPY_PTR_FROM_CONSTBUFFER(Name, Value, float)
	}

	void FDX12ShaderConstantsBuffer::UploadDataIfDity()
	{
		if (m_bIsDirty)
		{
			m_ConstantsTableBuffer->UploadCurrentBuffer();
			m_bIsDirty = false;
		}
	}


	FDX12ShaderBinder::FDX12ShaderBinder(FDX12Device& Device, FShaderBinderDesc& Desc)
	: m_Device(Device)
	, IShaderBinder(Desc)
	{
		InitMappers();
		BuildRootSignature();
		BuildDynamicHeap();
	}
	FDX12ShaderBinder::~FDX12ShaderBinder()
	{
	}

	void FDX12ShaderBinder::BindConstantsBuffer(uint32_t Slot, IShaderConstantsBuffer* Buffer)
	{
		FDX12ShaderConstantsBuffer* D3DBuffer = static_cast<FDX12ShaderConstantsBuffer*>(Buffer);
		D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = D3DBuffer->GetFrameResourceBuffer()->GetCurrentGPUAddress();
		Ref<FDX12CommandList> CommandList = m_Device.GetRenderCommandList();
		CommandList->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(Slot, GPUAddress);
	}

	void FDX12ShaderBinder::Bind()
	{
		Ref<FDX12CommandList> CommandList = m_Device.GetRenderCommandList();
		CommandList->SetGraphicsRootSignature(m_RootSignature);
	}

	void FDX12ShaderBinder::BuildRootSignature()
	{
		size_t ParamterCount = m_Desc.m_ConstantBufferLayouts.size() + m_Desc.m_TextureBufferLayouts.size();
		std::vector<CD3DX12_ROOT_PARAMETER> SlotRootParameter;
		SlotRootParameter.reserve(ParamterCount);
		
		UINT ParameterIndex = 0;
		for (FConstantBufferLayout& Layout : m_Desc.m_ConstantBufferLayouts)
		{
			SlotRootParameter[ParameterIndex].InitAsConstantBufferView(ParameterIndex);
			++ParameterIndex;
		}
		
		UINT SrvIndex = 0;
		std::vector<CD3DX12_DESCRIPTOR_RANGE> SrvTable;
		SrvTable.reserve(m_Desc.GetTextureBufferCount());
		for (FShaderResourceLayout& Layout : m_Desc.m_TextureBufferLayouts)
		{
			SrvTable[SrvIndex].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)Layout.GetSrvCount(), SrvIndex);
			SlotRootParameter[ParameterIndex].InitAsDescriptorTable(1, &SrvTable[SrvIndex], D3D12_SHADER_VISIBILITY_PIXEL);
			++ParameterIndex;
			++SrvIndex;
		}
		auto StaticSamplers = FDX12Device::GetStaticSamplers();
		CD3DX12_ROOT_SIGNATURE_DESC RoogSig(
			UINT(ParamterCount),
			SlotRootParameter.data(),
			UINT(StaticSamplers.size()),
			StaticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);
		ComPtr<ID3DBlob> SerializedRootSig = nullptr;
		ComPtr<ID3DBlob> ErrorBlob = nullptr;
		ThrowIfFailed(D3D12SerializeRootSignature(&RoogSig, D3D_ROOT_SIGNATURE_VERSION_1, &SerializedRootSig, &ErrorBlob));
		
		m_RootSignature = CreateRef<FDX12RootSignature>(m_Device, RoogSig);
	}
	void FDX12ShaderBinder::BuildDynamicHeap()
	{
		m_SrvDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(m_RootSignature.get());
		m_SrvDynamicDescriptorHeap->ParseRootSignature(D3DRootSignature->shared_from_this());
	}


	FDX12ShaderResourcesBuffer::FDX12ShaderResourcesBuffer(FDX12Device& Device, FShaderResourcesDesc& Desc, FDX12RootSignature* RootSignature)
		: IShaderResourcesBuffer(Desc)
		, m_Device(Device)
	{
		m_SrvDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		m_SrvDynamicDescriptorHeap->ParseRootSignature(RootSignature->shared_from_this());
	}

	FShaderResourcesDesc* FDX12ShaderResourcesBuffer::GetShaderResourceDesc()
	{
		return &m_Desc;
	}

	void FDX12ShaderResourcesBuffer::SetTexture2D(const std::string& Name, Ref<FTexture2D> Texture)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			FDX12Texture2D* D3DTexture = static_cast<FDX12Texture2D*>(Texture.get());
			m_SrvDynamicDescriptorHeap->StageDescriptors(Item.SRTIndex, Item.Offset, 1, D3DTexture->GetShaderResourceView());
			m_Desc.Mapper.SetTextureID(Name); 
		}
	}

	void FDX12ShaderResourcesBuffer::SetTextureCubemap(const std::string& Name, Ref<FTextureCubemap> Texture)
	{
	}

	void FDX12ShaderResourcesBuffer::UploadDataIfDirty()
	{
		if (m_bIsDirty)
		{
			Ref<FDX12CommandList> CommandList = m_Device.GetRenderCommandList();
			m_SrvDynamicDescriptorHeap->CommitStagedDescriptorsForDraw(*CommandList.get());
			m_bIsDirty = false;
		}
		else
		{
			//#todo
		}
	}
}
