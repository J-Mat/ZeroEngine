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

	std::vector<Ref<FMaterial>>& UMeshRenderComponent::GetPassMaterials(uint32_t LayerLayer)
	{
		CORE_ASSERT(m_RenderLayer & LayerLayer, "Must have this RenderLayer");
		//Ref<FTexture2D> Texture = TLibrary<FTexture2D>::Fetch("default");
		auto& iter = m_Materials.find(LayerLayer);
		if (iter == m_Materials.end() || m_Materials[LayerLayer].size() != m_SubmeshNum)
		{
			for (size_t i = m_Materials[LayerLayer].size(); i < m_SubmeshNum; i++)
			{
				Ref<FMaterial> Material = CreateRef<FMaterial>();
				m_Materials[LayerLayer].emplace_back(Material);
			}
		}	
		
		std::vector <Ref<FMaterial>>& OneLayerMaterials = m_Materials[LayerLayer];
		for (size_t i = 0; i < m_SubmeshNum; i++)
		{
			OneLayerMaterials[i]->SetShader(m_ShaderFile);
		}
		return m_Materials[LayerLayer];
	}

	void UMeshRenderComponent::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_MaterialHandle")
		{
			AttachParameters();
		}
		auto TextureCubmap = TLibrary<FTextureCubemap>::Fetch("default");
		if (TextureCubmap != nullptr)
		{
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				m_Materials[RENDERLAYER_OPAQUE][i]->SetTextureCubemap("gSkyboxMap", TextureCubmap);
			}
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
					m_Materials[RENDERLAYER_OPAQUE][i]->SetTexture2D(TextureName, Texture);
				}
			}
			m_ShaderFile = MaterialAsset->m_ShaderFile;
		}
	}
}
