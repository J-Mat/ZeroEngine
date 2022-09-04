#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"

namespace Zero
{
	UMeshRenderComponent::UMeshRenderComponent(uint32_t DeviceIndex)
		: UComponent(DeviceIndex)
	{
	}
	UMeshRenderComponent::~UMeshRenderComponent()
	{
	}
	std::vector<Ref<FMaterial>>& UMeshRenderComponent::GetPassMaterials(const EMeshRenderLayerType& LayerType)
	{
		auto& iter = Materials.find(LayerType);
		if (iter == Materials.end() || Materials[LayerType].size() != m_SubmeshNum)
		{
			for (int i = Materials[LayerType].size(); i < m_SubmeshNum; i++)
			{
				Materials[LayerType].emplace_back(nullptr);
			}
		}
		return Materials[LayerType];
	}
}
