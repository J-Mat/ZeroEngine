#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"
#include "Data/Asset/AssetManager.h"
#include "Data/Asset/AssetObject/MaterialAsset.h"

namespace Zero
{
	UMeshRenderComponent::UMeshRenderComponent()
		: UComponent()
	{
	}

	UMeshRenderComponent::~UMeshRenderComponent()
	{
	}

	void UMeshRenderComponent::PostInit()
	{
	}

	void UMeshRenderComponent::SetEnableMaterial(bool bEnable)
	{
		m_bEnableMaterial = bEnable;
		if (m_bEnableMaterial)
		{
			m_ClassInfoCollection.RemoveField("m_MaterialHandle", "Invisible");
		}
		else
		{
			m_ClassInfoCollection.AddField("m_MaterialHandle", "Invisible");
		}
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
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_MaterialHandle")
		{
			AttachParameters();
		}
	}
	void UMeshRenderComponent::AttachParameters()
	{
		if (!m_bEnableMaterial || m_MaterialHandle == "")
		{
			return;
		}
		UMaterialAsset* MaterialAsset = FAssetManager::GetInstance().Fetch<UMaterialAsset>(m_MaterialHandle);
		for (size_t i = 0; i < m_SubmeshNum; i++)
		{
			for (auto Iter : MaterialAsset->m_Textures)
			{
				const std::string& TextureName = Iter.first;
				Ref<FTexture2D> Texture = FAssetManager::GetInstance().FetchTexture(Iter.second);
				if (Texture != nullptr)
				{
					m_Materials[EMeshRenderLayerType::RenderLayer_Opaque][i]->SetTexture2D(TextureName, Texture);
				}
			}
			m_ShaderFile = MaterialAsset->m_ShaderFile;
		}
	}
}
