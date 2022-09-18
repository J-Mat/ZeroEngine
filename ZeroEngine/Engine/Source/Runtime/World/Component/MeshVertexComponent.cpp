#include "MeshVertexComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"

namespace Zero
{ 
	static FShaderConstantsDesc PerObtConstantsDesc;
	static FShaderConstantsDesc& GetPerObjectConstantsDesc()
	{
		if (PerObtConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_PerObjectConstants;
			int paraIndex = 0;

			PerObtConstantsDesc.Size = Layout.GetStride();
			for (auto Element : Layout)
			{
				PerObtConstantsDesc.Mapper.InsertConstant(Element, ERootParameters::PerObjCB);
			}
		}

		return PerObtConstantsDesc;
	}

	Ref<IShaderConstantsBuffer> FPerObjectConstantsBufferPool::GetPerObjectConstantsBuffer(UCoreObject* Obj)
	{
		Ref<IShaderConstantsBuffer> Result;
		if (m_IdleConstantsBuffer.empty())
		{
			auto Device = Obj->GetWorld()->GetDevice();
			Result = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(Device.get(), GetPerObjectConstantsDesc());
		}
		else
		{
			Result = m_IdleConstantsBuffer.front();
			m_IdleConstantsBuffer.pop();
		}

		return Result;
	}

	void FPerObjectConstantsBufferPool::PushToPool(Ref<IShaderConstantsBuffer> Buffer)
	{
		m_IdleConstantsBuffer.push(Buffer);
	}


	UMeshVertexComponent::UMeshVertexComponent(FMeshType& MeshType)
		: UComponent()
		, m_MeshType(MeshType)
	{
	}

	void UMeshVertexComponent::PostInit()
	{
		m_ShaderConstantsBuffer = FPerObjectConstantsBufferPool::GetInstance().GetPerObjectConstantsBuffer(this);		
	}

	UMeshVertexComponent::~UMeshVertexComponent()
	{
		FPerObjectConstantsBufferPool::GetInstance().PushToPool(m_ShaderConstantsBuffer);
	}
	void UMeshVertexComponent::CreateMesh()
	{
		m_Mesh = FRenderer::GraphicFactroy->CreateMesh(
			GetWorld()->GetDevice().get(),
			m_MeshData.Vertices.data(),
			uint32_t(m_MeshData.Vertices.size()),
			m_MeshData.Indices.data(),
			uint32_t(m_MeshData.Indices.size()) ,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
	}
}
