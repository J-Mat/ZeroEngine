#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"
#include "Data/Asset/AssetManager.h"
#include "Data/Asset/AssetObject/MaterialAsset.h"
#include "Render/RHI/PipelineStateObject.h"

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
			m_ClassInfoCollection.RemoveField("m_Psotype", "Invisible");
		}
		else
		{
			m_ClassInfoCollection.AddField("m_MaterialHandle", "Invisible");
			m_ClassInfoCollection.AddField("m_Psotype", "Invisible");
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
		Ref<IShader> ShaderFile = m_PipelineStateObject != nullptr ? 
			m_PipelineStateObject->GetPSODescriptor().Shader : 
			TLibrary<FPipelineStateObject>::Fetch(PSO_FORWARDLIT)->GetPSODescriptor().Shader;
		for (size_t i = 0; i < m_SubmeshNum; i++)
		{
			OneLayerMaterials[i]->SetShader(ShaderFile);
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
		else if (Property->GetPropertyName() == "m_ColorParameter" || Property->GetPropertyName() == "m_FloatSliderParameter")
		{
			AttachParameters();
		}
		else if (m_bEnableMaterial && Property->GetPropertyName() == "m_Psotype")
		{
			SwitchPso();
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
		if (!m_bEnableMaterial)
		{
			return;
		}
		int32_t RenderLayer = m_RenderLayer;
		if (m_MaterialHandle != "")
		{
			UMaterialAsset* MaterialAsset = FAssetManager::GetInstance().Fetch<UMaterialAsset>(m_MaterialHandle);
			while (RenderLayer > 0)
			{
				uint32_t CurLayer = (RenderLayer & (-RenderLayer));
				RenderLayer ^= CurLayer;

				for (size_t i = 0; i < m_SubmeshNum; i++)
				{
					for (auto Iter : MaterialAsset->m_Textures)
					{
						const std::string& TextureName = Iter.first;
						Ref<FTexture2D> Texture = FAssetManager::GetInstance().FetchTexture(Iter.second);
						if (Texture != nullptr)
						{
							m_Materials[CurLayer][i]->SetTexture2D(TextureName, Texture);
						}
					}
				}
			}
		}
			
		
		RenderLayer = m_RenderLayer;
		while (RenderLayer > 0)
		{
			uint32_t CurLayer = (RenderLayer & (-RenderLayer));
			RenderLayer ^= CurLayer;

			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				for (auto Iter : m_ColorParameter)
				{
					m_Materials[CurLayer][i]->SetFloat3(Iter.first, Iter.second);
				}

				for (auto Iter : m_FloatSliderParameter)
				{
					m_Materials[CurLayer][i]->SetFloat(Iter.first, Iter.second.Value);
				}
			}
		}
	}
	void UMeshRenderComponent::SwitchPso()
	{
		switch (m_Psotype)
		{
		case Zero::PT_ForwardLit:
			m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PSO_FORWARDLIT);
			break;
		case Zero::PT_Fresnel:
			m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PSO_FRESNEL);
			break;
		default:
			break;
		}
	}
}
