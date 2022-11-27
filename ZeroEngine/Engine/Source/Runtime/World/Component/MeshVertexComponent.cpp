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
				PerObtConstantsDesc.Mapper.InsertConstant(Element);
			}
		}

		return PerObtConstantsDesc;
	}

	Ref<IShaderConstantsBuffer> FPerObjectConstantsBufferPool::GetPerObjectConstantsBuffer(UCoreObject* Obj)
	{
		Ref<IShaderConstantsBuffer> Result;
		if (m_IdleConstantsBuffer.empty())
		{
			Result = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(GetPerObjectConstantsDesc());
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


	UMeshVertexComponent::UMeshVertexComponent()
		: UComponent()
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
			m_MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
	}
	void UMeshVertexComponent::GenerateAABB()
	{
		for (FMeshData& MeshData : m_MeshDatas)
		{
			m_Mesh->Merge(MeshData.AABB);
		}
	}
}
