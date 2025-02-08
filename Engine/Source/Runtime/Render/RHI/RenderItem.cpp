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

	Ref<FComputeItem> FComputeRenderItemPool::Request()
	{
		Ref<FComputeItem> Item = nullptr;
		if (m_AvailableRenderItems.size() != 0)
		{
			Item = m_AvailableRenderItems.back();
			m_AvailableRenderItems.pop_back();
		}
		else
		{
			Item = CreateRef<FComputeItem>();
		}
		Item->Reset();
		return Item;
	}

	void FComputeRenderItemPool::Push(Ref<FComputeItem> Item)
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
	
	Zero::Ref<Zero::FGraphicPipelineStateObject> FRenderItem::GetPSObj(const FRenderParams& RenderSettings) {
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

	Ref<Zero::FGraphicPipelineStateObject> FRenderItem::GetPSObj()
	{
		m_PipelineStateObject = FPSOCache::Get().FetchGraphicPso(m_PsoID);
		return m_PipelineStateObject;
	}
	
	

	bool FRenderItem::CanRender(FCommandListHandle ComamndListHandle, const FRenderParams& RenderSettings)
	{
		auto PsoObj = GetPSObj(RenderSettings);
		if (PsoObj == nullptr)
		{
			return false;
		}
		PsoObj->Bind(ComamndListHandle);
		m_ShaderParamsSettings->SetShader(PsoObj->GetPSODescriptor().Shader);
		m_ShaderParamsSettings->Tick();
		m_ShaderParamsSettings->SetPass(ComamndListHandle, ERenderPassType::Graphics);
		return true;
	}



	void FRenderItem::Render(FCommandListHandle ComamndListHandle)
	{
		m_ShaderParamsSettings->OnDrawCall(ComamndListHandle, ERenderPassType::Graphics);
		if (m_PerObjectBuffer != nullptr)
		{
			static auto PerObjIndex = m_ShaderParamsSettings->GetShader()->GetBinder()->GetBinderDesc().m_PerObjIndex ;
			m_ShaderParamsSettings->GetShader()->GetBinder()->BindConstantsBuffer(ComamndListHandle, PerObjIndex, m_PerObjectBuffer.get());
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
		m_ShaderParamsSettings->PostDrawCall();
	}
	
	FComputeItem::FComputeItem()
	{
		m_ShaderParamsGroup = CreateRef<FShaderParamsSettings>();
	}

	void FComputeItem::Reset()
	{
		m_PipelineStateObject = nullptr;
	}

	Ref<FComputePipelineStateObject> FComputeItem::GetPsoObj()
	{
		m_PipelineStateObject = FPSOCache::Get().FetchComputePso(m_PsoID);
		return m_PipelineStateObject;
	}
	
	void FComputeItem::PreDispatch(FCommandListHandle ComamndListHandle)
	{
		auto PsoObj = GetPsoObj();
		if (PsoObj == nullptr)
		{
			return;
		}
		PsoObj->Bind(ComamndListHandle);
		m_ShaderParamsGroup->SetShader(PsoObj->GetPSODescriptor().Shader);
		m_ShaderParamsGroup->Tick();
		m_ShaderParamsGroup->SetPass(ComamndListHandle, ERenderPassType::Compute);
	}

	void FComputeItem::Dispatch(FCommandListHandle ComamndListHandle, uint32_t ThreadNumX, uint32_t ThreadNumY, uint32_t ThreadNumZ)
	{
		m_ShaderParamsGroup->OnDrawCall(ComamndListHandle, ERenderPassType::Compute);
		Ref<FCommandList> RHICommandList = FGraphic::GetDevice()->GetRHICommandList(ComamndListHandle);
		uint32_t GroupNumX = ThreadNumX / m_PipelineStateObject->GetPSODescriptor().BlockSize_X;
		uint32_t GroupNumY = ThreadNumX / m_PipelineStateObject->GetPSODescriptor().BlockSize_Y;
		uint32_t GroupNumZ = ThreadNumX / m_PipelineStateObject->GetPSODescriptor().BlockSize_Z;
		RHICommandList->Dispatch(GroupNumX, GroupNumY, GroupNumZ);
	}
}
