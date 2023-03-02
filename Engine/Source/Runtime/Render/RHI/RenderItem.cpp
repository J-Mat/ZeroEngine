#include "RenderItem.h"
#include "./Shader/Shader.h"
#include "./Shader/ShaderBinder.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/PipelineStateObject.h"

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
		return Item;
	}

	void FRenderItemPool::Push(Ref<FRenderItem> Item)
	{
		m_RenderItems.push_back(Item);
	}


	FRenderItem::FRenderItem(Ref<FMesh> Mesh)
	{

	}

	FRenderItem::FRenderItem(Ref<FMesh> Mesh, const FSubMesh& SubMesh)
	:m_Mesh(Mesh)
	,m_SubMesh(SubMesh)
	{
	}

	FRenderItem::~FRenderItem()
	{
	}
	void FRenderItem::SetModelMatrix(const ZMath::mat4& Transform)
	{
		m_Transform = Transform;
		m_PerObjectBuffer->SetMatrix4x4("Model", Transform);
	}
	
	void FRenderItem::PreRender(FCommandListHandle ComamndListHandle)
	{
		m_PipelineStateObject->Bind(ComamndListHandle);
		m_Material->Tick();
		m_Material->SetPass(ComamndListHandle);
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
	}
}
