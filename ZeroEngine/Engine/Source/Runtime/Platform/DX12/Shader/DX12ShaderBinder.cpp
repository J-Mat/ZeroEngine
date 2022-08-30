#include "DX12ShaderBinder.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/RootSignature.h"
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

	void FDX12ShaderConstantsBuffer::UploadDataIfDity(IShaderBinder* m_ShaderBinder)
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
	void FDX12ShaderBinder::Bind(uint32_t Slot)
	{
		Ref<FDX12CommandList> CommandList = m_Device.GetActiveCommandList(Slot);
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
			ParamterCount,
			SlotRootParameter.data(),
			StaticSamplers.size(),
			StaticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);
		ComPtr<ID3DBlob> SerializedRootSig = nullptr;
		ComPtr<ID3DBlob> ErrorBlob = nullptr;
		ThrowIfFailed(D3D12SerializeRootSignature(&RoogSig, D3D_ROOT_SIGNATURE_VERSION_1, &SerializedRootSig, &ErrorBlob));
		
		m_RootSignature = CreateRef<FRootSignature>(m_Device, RoogSig);
	}
	void FDX12ShaderBinder::BuildDynamicHeap()
	{
		m_SrvDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		m_SrvDynamicDescriptorHeap->ParseRootSignature(m_RootSignature);
	}
}
