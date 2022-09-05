#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"

namespace Zero
{
	UMeshRenderComponent::UMeshRenderComponent()
		: UComponent()
	{
	}
	UMeshRenderComponent::~UMeshRenderComponent()
	{
	}
	std::vector<Ref<FMaterial>>& UMeshRenderComponent::GetPassMaterials(const EMeshRenderLayerType& LayerType)
	{
		auto& iter = m_Materials.find(LayerType);
		if (iter == m_Materials.end() || m_Materials[LayerType].size() != m_SubmeshNum)
		{
			for (int i = m_Materials[LayerType].size(); i < m_SubmeshNum; i++)
			{
				m_Materials[LayerType].emplace_back(nullptr);
			}
		}
		return m_Materials[LayerType];
	}
}
