#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"

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
			for (size_t i = m_Materials[LayerType].size(); i < m_SubmeshNum; i++)
			{
				Ref<FMaterial> Material = CreateRef<FMaterial>();
				m_Materials[LayerType].emplace_back(Material);
			}
		}
		return m_Materials[LayerType];
	}
}
