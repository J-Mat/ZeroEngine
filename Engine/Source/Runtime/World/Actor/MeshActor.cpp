#include "MeshActor.h"
#include "Render/RendererAPI.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/RHI/RenderItem.h"
#include "Render/RHI/Mesh.h"
#include "World/World.h"
#include "Render/Moudule/ShaderParamsSettings.h"

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
		m_MeshRenderComponent->UpdateSettings();
	}

	void UMeshActor::CommitToPipieline()
	{
		int32_t RenderLayer = m_MeshRenderComponent->GetRenderLayer();
		
		const auto& LayerInfo = m_MeshRenderComponent->GetLayerInfo();
		for (const auto Iter : LayerInfo)
		{
			ERenderLayer Layer = Iter.first;
			auto RenderItemPool = m_World->GetRenderItemPool(Layer);
			uint32_t MaterialIndex = 0;
			for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
			{
				Ref<FRenderItem> Item = RenderItemPool->Request();
				Item->m_PsoID = Iter.second->PsoID;
				Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
				Item->m_SubMesh = SubMesh;
				Item->m_PerObjectBuffer = m_MeshRenderComponent->GetAndPreDraCallPerObjConstanBuffer();
				Item->m_ShaderParamsSettings = Iter.second->ShaderParamsSettings[MaterialIndex];
				Item->SetModelMatrix(m_TransformationComponent->GetTransform());
				MaterialIndex++;

				RenderItemPool->Push(Item);
			}
			RenderItemPool->AddGuid(m_GUID);
		}
	}

	void UMeshActor::Tick()
	{
		Supper::Tick();
		SetCustomParemeters();
		CommitToPipieline();
	}

	ZMath::FAABB UMeshActor::GetAABB()
	{
		return m_MeshVertexComponent->m_Mesh->GetAABB();
	}

	void UMeshActor::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, ERenderLayer RenderLayer)
	{
		m_MeshRenderComponent->SetParameter(ParameterName, ShaderDataType, ValuePtr, RenderLayer);
	}
}