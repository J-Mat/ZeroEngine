#include "RenderItem.h"
#include "Shader.h"
#include "ShaderBinder.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/Mesh.h"

namespace Zero
{
	FDrawItem::FDrawItem(Ref<FMesh> Mesh)
	{
	}
	FDrawItem::FDrawItem(Ref<FMesh> Mesh, Ref<FSubMesh> SubMesh)
	:m_Mesh(Mesh)
	,m_SubMesh(SubMesh)
	{
	}
	FDrawItem::~FDrawItem()
	{
	}
	void FDrawItem::SetModelMatrix(const ZMath::mat4& Transform)
	{
		m_ConstantsBuffer->SetMatrix4x4("Model", Transform);
	}
	void FDrawItem::OnDrawCall()
	{
		m_Material->GetShader()->GetBinder()->BindConstantsBuffer(ERootParameters::PerObjCB, m_ConstantsBuffer.get());
		if (m_SubMesh == nullptr)
		{
			m_Mesh->Draw();
		}
		else
		{
			m_Mesh->DrawSubMesh(*m_SubMesh.get());
		}
	}
}
