#include "MeshActor.h"
#include "Render/RendererAPI.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/RHI/RenderItem.h"
#include "Render/RHI/Mesh.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"

namespace Zero
{
	UMeshActor::UMeshActor()
		: UActor()
	{
		m_MeshVertexComponent = CreateComponent<UMeshVertexComponent>(this);
		m_MeshRenderComponent = CreateComponent<UMeshRenderComponent>(this);
	}

	void UMeshActor::PostInit()
	{
		Supper::PostInit();
		BuildMesh();
		m_MeshVertexComponent->GenerateAABB();

		uint32_t SubMeshNum = m_MeshVertexComponent->m_Mesh->GetSubMeshNum();
		m_MeshRenderComponent->SetSubmeshNum(SubMeshNum);
		m_MeshRenderComponent->AttachParameters();
	}

	void UMeshActor::CommitToPipieline()
	{
		int32_t RenderLayer = m_MeshRenderComponent->m_RenderLayer;
		
		while (RenderLayer > 0)
		{
			uint32_t CurLayer = (RenderLayer & (-RenderLayer));
			RenderLayer ^= CurLayer;
			auto RenderItemPool = m_World->GetRenderItemPool(CurLayer);
			uint32_t MaterialIndex = 0;
			for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
			{
				Ref<FRenderItem> Item = RenderItemPool->Request();
				Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
				Item->m_SubMesh = SubMesh;
				Item->m_MaterialBuffer = m_MeshVertexComponent->m_PerObjConstantsBuffer;
				Item->m_Material = m_MeshRenderComponent->GetPassMaterials(CurLayer)[MaterialIndex];
				Item->m_PipelineStateObject = m_MeshRenderComponent->GetPipelineStateObject();
				Item->m_Material->SetShader(Item->m_PipelineStateObject->GetPSODescriptor().Shader);
				Item->SetModelMatrix(m_TransformationComponent->GetTransform());
				MaterialIndex++;

				RenderItemPool->Push(Item);
			}
			RenderItemPool->AddGuid(m_GUID);
		}
	}

	void UMeshActor::Tick()
	{
		CommitToPipieline();
	}

	ZMath::FAABB UMeshActor::GetAABB()
	{
		return m_MeshVertexComponent->m_Mesh->GetAABB();
	}

	void UMeshActor::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr)
	{
		m_MeshRenderComponent->SetParameter(ParameterName, ShaderDataType, ValuePtr);
	}

}