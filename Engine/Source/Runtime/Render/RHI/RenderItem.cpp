#include "RenderItem.h"
#include "./Shader/Shader.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/GraphicDevice.h"
#include "./Shader/ShaderBinder.h"
#include "Render/Moudule/PSOCache.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Render/RHI/CommandList.h"

namespace Zero
{
	Ref<FRenderItemPool> FRenderItemPool::s_DIYRenderItemPool = CreateRef<FRenderItemPool>();


	void FRenderItemPool::Reset()
	{
		m_AvailableRenderItems.insert(
			m_AvailableRenderItems.end(),
			m_RenderItems.begin(),
			m_RenderItems.end()
		);
		
		m_RenderGuids.clear();
		m_RenderItems.clear();
	}
	Ref<FRenderItem> FRenderItemPool::Request()
	{
		Ref<FRenderItem> Item = nullptr;
		if (m_AvailableRenderItems.size() != 0)
		{
			Item = m_AvailableRenderItems.back();
			m_AvailableRenderItems.pop_back();
		}
		else
		{
			Item = CreateRef<FRenderItem>();
		}
		Item->Reset();
		return Item;
	}

	void FRenderItemPool::Push(Ref<FRenderItem> Item)
	{
		m_RenderItems.push_back(Item);
	}


	void FComputeRenderItemPool::Reset()
	{
		m_AvailableRenderItems.insert(
			m_AvailableRenderItems.end(),
			m_ComputeRenderItems.begin(),
			m_ComputeRenderItems.end()
		);
		
		m_ComputeRenderItems.clear();
	}

	Ref<FComputeRenderItem> FComputeRenderItemPool::Request()
	{
		Ref<FComputeRenderItem> Item = nullptr;
		if (m_AvailableRenderItems.size() != 0)
		{
			Item = m_AvailableRenderItems.back();
			m_AvailableRenderItems.pop_back();
		}
		else
		{
			Item = CreateRef<FComputeRenderItem>();
		}
		Item->Reset();
		return Item;
	}

	void FComputeRenderItemPool::Push(Ref<FComputeRenderItem> Item)
	{
		m_ComputeRenderItems.push_back(Item);
	}


	FRenderItem::FRenderItem(Ref<FMesh> Mesh)
	{

	}

	FRenderItem::FRenderItem(Ref<FMesh> Mesh, const FSubMesh& SubMesh)
	:m_Mesh(Mesh)
	,m_SubMesh(SubMesh)
	{
	}

	void FRenderItem::Reset()
	{
		m_PipelineStateObject = nullptr;
	}

	FRenderItem::~FRenderItem()
	{
	}
	void FRenderItem::SetModelMatrix(const ZMath::mat4& Transform)
	{
		m_Transform = Transform;
		m_PerObjectBuffer->SetMatrix4x4("Model", Transform);
	}
	
	Zero::Ref<Zero::FGraphicPipelineStateObject> FRenderItem::GetPsoObj(const FRenderParams& RenderSettings)
{
		switch (RenderSettings.PiplineStateMode)
		{
		case EPiplineStateMode::Dependent:
			m_PipelineStateObject = FPSOCache::Get().FetchGraphicPso(m_PsoID);
			break;
		case EPiplineStateMode::AllSpecific:
			m_PipelineStateObject = FPSOCache::Get().FetchGraphicPso(RenderSettings.PsoID);
			break;
		case EPiplineStateMode::Only:
			if (m_PsoID == RenderSettings.PsoID)
			{
				return nullptr;
			}
			m_PipelineStateObject = FPSOCache::Get().FetchGraphicPso(m_PsoID);
			break;
		default:
			break;
		}
		return m_PipelineStateObject;
	}

	Ref<Zero::FGraphicPipelineStateObject> FRenderItem::GetPsoObj()
	{
		m_PipelineStateObject = FPSOCache::Get().FetchGraphicPso(m_PsoID);
		return m_PipelineStateObject;
	}
	
	

	bool FRenderItem::CanRender(FCommandListHandle ComamndListHandle, const FRenderParams& RenderSettings)
	{
		auto PsoObj = GetPsoObj(RenderSettings);
		if (PsoObj == nullptr)
		{
			return false;
		}
		PsoObj->Bind(ComamndListHandle);
		m_Material->SetShader(PsoObj->GetPSODescriptor().Shader);
		m_Material->Tick();
		m_Material->SetPass(ComamndListHandle);
		return true;
	}



	void FRenderItem::Render(FCommandListHandle ComamndListHandle)
	{
		m_Material->OnDrawCall(ComamndListHandle);
		if (m_PerObjectBuffer != nullptr)
		{
			static auto PerObjIndex = m_Material->GetShader()->GetBinder()->GetBinderDesc().m_PerObjIndex ;
			m_Material->GetShader()->GetBinder()->BindConstantsBuffer(ComamndListHandle, PerObjIndex, m_PerObjectBuffer.get());
			m_PerObjectBuffer->UploadDataIfDirty();
		}
		if (m_SubMesh.IsNull())
		{
			m_Mesh->Draw(ComamndListHandle);
		}
		else
		{
			m_Mesh->DrawSubMesh(m_SubMesh, ComamndListHandle);
		}
		m_Material->PostDrawCall();
	}
	
	FComputeRenderItem::FComputeRenderItem()
	{
		m_ShaderParamsGroup = CreateRef<FShaderParamsGroup>();
	}

	void FComputeRenderItem::Reset()
	{
		m_PipelineStateObject = nullptr;
	}

	Ref<FComputePipelineStateObject> FComputeRenderItem::GetPsoObj()
	{
		m_PipelineStateObject = FPSOCache::Get().FetchComputePso(m_PsoID);
		return m_PipelineStateObject;
	}
	
	void FComputeRenderItem::Compute(FCommandListHandle ComamndListHandle, uint32_t NumGroupsX, uint32_t NumGroupsY, uint32_t NumGroupsZ)
	{
		auto PsoObj = GetPsoObj();
		if (PsoObj == nullptr)
		{
			return;
		}
		PsoObj->Bind(ComamndListHandle);
		m_ShaderParamsGroup->SetShader(PsoObj->GetPSODescriptor().Shader);
		m_ShaderParamsGroup->Tick();
		m_ShaderParamsGroup->SetPass(ComamndListHandle);

		Ref<FCommandList> RHICommandList = FGraphic::GetDevice()->GetRHICommandList(ComamndListHandle);
		RHICommandList->Dispatch(NumGroupsX, NumGroupsY, NumGroupsZ);
	}
}
