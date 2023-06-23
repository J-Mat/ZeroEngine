#include "DX12ShaderBinder.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Platform/DX12/MemoryManage/Descriptor/DynamicDescriptorHeap.h"
#include "../DX12CommandList.h"
#include "../DX12TextureCube.h"

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
		m_ConstantsTableBuffer = CreateRef<FFrameResourceBuffer>(m_Desc.Size, Desc.bDynamic);
	}

	void FDX12ShaderConstantsBuffer::PreDrawCall()
	{
		if (m_Desc.bDynamic)
		{
			m_ConstantsTableBuffer->AllocateDynamicSpace();
		}
	}

	void FDX12ShaderConstantsBuffer::SetInt(const std::string& Name, int Value)
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

	void FDX12ShaderConstantsBuffer::UploadDataIfDirty()
	{
		if (m_bIsDirty)
		{
			m_ConstantsTableBuffer->UploadCurrentBuffer();
			m_bIsDirty = false;
		}
	}



	FDX12ShaderBinder::FDX12ShaderBinder(FShaderBinderDesc& Desc)
	: IShaderBinder(Desc)
	{
		InitMappers();
		BuildRootSignature();
		BuildDynamicHeap();
	}
	FDX12ShaderBinder::~FDX12ShaderBinder()
	{
	}

	void FDX12ShaderBinder::BindConstantsBuffer(FCommandListHandle CommandListHandle, int32_t Slot, IShaderConstantsBuffer* Buffer)
	{
		if (Slot >= 0)
		{
			FDX12ShaderConstantsBuffer* D3DBuffer = static_cast<FDX12ShaderConstantsBuffer*>(Buffer);
			D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = D3DBuffer->GetFrameResourceBuffer()->GetCurrentGPUAddress();
			Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
			CommandList->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(Slot, GPUAddress);
		}
	}

	void FDX12ShaderBinder::Bind(FCommandListHandle CommandListHandle, ERenderPassType RenderPassUsage)
	{
		Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		switch (RenderPassUsage)
		{
		case ERenderPassType::Graphics:
			CommandList->SetGraphicsRootSignature(m_RootSignature);
			break;
		case ERenderPassType::Compute:
			CommandList->SetComputeRootSignature(m_RootSignature);
			break;
		default:
			break;
		}
		
	}


	void FDX12ShaderBinder::BuildRootSignature()
	{
		size_t ParamterCount = m_Desc.m_ConstantBufferLayouts.size() + m_Desc.m_SRVResourceLayout.GetSrvCount() + m_Desc.m_UAVResourceLayout.GetUavCount();
		std::vector<CD3DX12_ROOT_PARAMETER1> SlotRootParameter;
		SlotRootParameter.resize(ParamterCount);
		
		UINT ParameterIndex = 0;
		for (FConstantBufferLayout& Layout : m_Desc.m_ConstantBufferLayouts)
		{
			SlotRootParameter[ParameterIndex].InitAsConstantBufferView(Layout.GetBindPoint());
			++ParameterIndex;
		}
		
			UINT SrvIndex = 0;
			std::vector<CD3DX12_DESCRIPTOR_RANGE1> SrvTable(m_Desc.m_SRVResourceLayout.GetSrvCount());
			for (FSRVElement& Element : m_Desc.m_SRVResourceLayout)
			{
				
				SrvTable[SrvIndex].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)Element.TextureNum, Element.BindPoint, Element.RegisterSpace,
					m_Desc.m_RenderPassType == ERenderPassType::Graphics ? D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE : D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
				SlotRootParameter[ParameterIndex].InitAsDescriptorTable(1, &SrvTable[SrvIndex], 
					m_Desc.m_RenderPassType == ERenderPassType::Graphics ? D3D12_SHADER_VISIBILITY_PIXEL : D3D12_SHADER_VISIBILITY_ALL);
				++ParameterIndex;
				++SrvIndex;
			}

			UINT UavIndex = 0;
			std::vector<CD3DX12_DESCRIPTOR_RANGE1> UavTable(m_Desc.m_UAVResourceLayout.GetUavCount());
			for (FUAVElement& Element : m_Desc.m_UAVResourceLayout)
			{
				UavTable[UavIndex].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, (UINT)Element.TextureNum, Element.BindPoint, Element.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
				SlotRootParameter[ParameterIndex].InitAsDescriptorTable(1, &UavTable[UavIndex], D3D12_SHADER_VISIBILITY_ALL);
				++ParameterIndex;
				++UavIndex;
			}

		D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
		auto StaticSamplers = FDX12Device::GetStaticSamplers(m_Desc.m_ShaderSamplers);
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc(
			UINT(ParamterCount), 
			SlotRootParameter.data(), 
			UINT(StaticSamplers.size()), 
			StaticSamplers.data(),
			RootSignatureFlags
		); 
		/*
		CD3DX12_ROOT_SIGNATURE_DESC RoogSig(
			UINT(ParamterCount),
			SlotRootParameter.data(),
			UINT(StaticSamplers.size()),
			StaticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);
		*/
		m_RootSignature.reset();
		m_RootSignature = CreateRef<FDX12RootSignature>(RootSignatureDesc.Desc_1_1);
		
		//m_RootSignature = CreateRef<FDX12RootSignature>(RoogSig);
	}

	void FDX12ShaderBinder::BuildDynamicHeap()
	{
		FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(m_RootSignature.get());
	}


	FDX12ShaderResourcesBuffer::FDX12ShaderResourcesBuffer(FShaderResourcesDesc& Desc, FDX12RootSignature* RootSignature)
		: IShaderResourcesBuffer(Desc)
	{
		m_SrvDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerDynamicDescriptorHeap = CreateRef<FDynamicDescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		m_SrvDynamicDescriptorHeap->ParseRootSignature(RootSignature->AsShared());
	}

	FShaderResourcesDesc* FDX12ShaderResourcesBuffer::GetShaderResourceDesc()
	{
		return &m_Desc;
	}

	void FDX12ShaderResourcesBuffer::SetTexture2D(const std::string& Name, FTexture2D* Texture)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			FDX12Texture2D* D3DTexture = static_cast<FDX12Texture2D*>(Texture);
			FDX12ShaderResourceView* Srv = static_cast<FDX12ShaderResourceView*>(D3DTexture->GetSRV());
			m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Item.Offset, 1, Srv->GetDescriptorHandle());
			m_Desc.Mapper.SetTextureID(Name); 
			m_bIsDirty = true;
		}
	}

	void FDX12ShaderResourcesBuffer::SetBuffer(const std::string& Name, FBuffer* Buffer)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			FDX12Buffer* D3DTexture = static_cast<FDX12Buffer*>(Buffer);
			FDX12ShaderResourceView* Srv = static_cast<FDX12ShaderResourceView*>(D3DTexture->GetSRV());
			m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Item.Offset, 1, Srv->GetDescriptorHandle());
			m_Desc.Mapper.SetTextureID(Name, EShaderResourceType::Buffer);
			m_bIsDirty = true;
		}
	}

	void FDX12ShaderResourcesBuffer::SetTexture2DArray(const std::string& Name, std::vector<FTexture2D*> Textures)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			uint32_t Offset = 0;
			for (auto Texture : Textures)
			{
				FDX12Texture2D* D3DTexture = static_cast<FDX12Texture2D*>(Texture);
				FDX12ShaderResourceView* Srv = static_cast<FDX12ShaderResourceView*>(D3DTexture->GetSRV());
				m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Offset++, 1, Srv->GetDescriptorHandle());
			}
			m_Desc.Mapper.SetTextureID(Name, EShaderResourceType::Texture2D);
			m_bIsDirty = true;
		}
	}

	void FDX12ShaderResourcesBuffer::SetTextureCube(const std::string& Name, FTextureCube* Texture)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			FDX12TextureCube* D3DTexture = static_cast<FDX12TextureCube*>(Texture);
			FDX12ShaderResourceView* Srv = static_cast<FDX12ShaderResourceView*>(D3DTexture->GetSRV());
			m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Item.Offset, 1, Srv->GetDescriptorHandle());
			m_Desc.Mapper.SetTextureID(Name, EShaderResourceType::Cubemap);
			m_bIsDirty = true;
		}
	}

	void FDX12ShaderResourcesBuffer::SetTextureCubemapArray(const std::string& Name, const std::vector<Ref<FTextureCube>>& TextureCubes)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			uint32_t Offset = 0;
			for (auto Texture : TextureCubes)
			{
				FDX12TextureCube* D3DTexture = static_cast<FDX12TextureCube*>(Texture.get());
				FDX12ShaderResourceView* Srv = static_cast<FDX12ShaderResourceView*>(D3DTexture->GetSRV());
				m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Offset++, 1, Srv->GetDescriptorHandle());
			}
			m_Desc.Mapper.SetTextureID(Name, EShaderResourceType::Cubemap);
			m_bIsDirty = true;
		}

	}

	void FDX12ShaderResourcesBuffer::SetTexture2D_Uav(const std::string& Name, FTexture2D* Texture)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			FDX12Texture2D* D3DTexture = static_cast<FDX12Texture2D*>(Texture);
			FDX12UnorderedAccessResourceView* Srv = static_cast<FDX12UnorderedAccessResourceView*>(D3DTexture->GetUAV());
			m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Item.Offset, 1, Srv->GetDescriptorHandle());
			m_Desc.Mapper.SetTextureID(Name, EShaderResourceType::Texture2D_Uav);
			m_bIsDirty = true;
		}
	}

	void FDX12ShaderResourcesBuffer::SetBuffer_Uav(const std::string& Name, FBuffer* Buffer)
	{
		FShaderResourceItem Item;
		if (m_Desc.Mapper.FetchResource(Name, Item))
		{
			FDX12Buffer* D3DBuffer = static_cast<FDX12Buffer*>(Buffer);
			FDX12UnorderedAccessResourceView* Uav = static_cast<FDX12UnorderedAccessResourceView*>(D3DBuffer->GetUAV());
			m_SrvDynamicDescriptorHeap->StageDescriptors(Item.ParamsIndex, Item.Offset, 1, Uav->GetDescriptorHandle());
			m_Desc.Mapper.SetTextureID(Name, EShaderResourceType::Buffer_Uav);
			m_bIsDirty = true;
		}

	}

	void FDX12ShaderResourcesBuffer::UploadDataIfDirty(FCommandListHandle CommandListHandle, ERenderPassType RenderPassUsage)
	{
		switch (RenderPassUsage)
		{
		case ERenderPassType::Graphics:
			m_SrvDynamicDescriptorHeap->SetAsShaderResourceHeap(CommandListHandle);
			break;
		case ERenderPassType::Compute:
			m_SrvDynamicDescriptorHeap->SetComputeAsShaderResourceHeap(CommandListHandle);
			break;
		default:
			break;
		}
	}
}
