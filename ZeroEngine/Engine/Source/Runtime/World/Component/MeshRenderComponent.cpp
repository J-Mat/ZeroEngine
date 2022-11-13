#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetObject/MaterialAsset.h"

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
		//Ref<FTexture2D> Texture = TLibrary<FTexture2D>::Fetch("default");
		auto& iter = m_Materials.find(LayerType);
		if (iter == m_Materials.end() || m_Materials[LayerType].size() != m_SubmeshNum)
		{
			for (size_t i = m_Materials[LayerType].size(); i < m_SubmeshNum; i++)
			{
				Ref<FMaterial> Material = CreateRef<FMaterial>();
				m_Materials[LayerType].emplace_back(Material);
			}
		}	
		for (size_t i = 0; i < m_SubmeshNum; i++)
		{
			m_Materials[LayerType][i]->SetShader(m_ShaderFile);
		}
		return m_Materials[LayerType];
	}

	void UMeshRenderComponent::PostEdit(UProperty* Property)
	{
		if (Property->GetPropertyName() == "m_MaterialHandle")
		{
			UMaterialAsset* MaterialAsset = FAssetManager::GetInstance().Fetch<UMaterialAsset>(m_MaterialHandle);
			Ref<FTexture2D> Texture = TLibrary<FTexture2D>::Fetch(MaterialAsset->m_Diffuse);
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				m_Materials[EMeshRenderLayerType::RenderLayer_Opaque][i]->SetTexture2D("gDiffuseMap", Texture);
				m_ShaderFile = MaterialAsset->m_ShaderFile;
			}
		}
	}
}
