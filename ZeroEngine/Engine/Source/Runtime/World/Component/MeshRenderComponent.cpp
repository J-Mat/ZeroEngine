#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"
#include "Data/Asset/AssetManager.h"
#include "World/Object/PropertyObject.h"
#include "World/Object/MapPropretyObject.h"
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
		return m_Materials[LayerLayer];
	}

	void UMeshRenderComponent::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr)
	{
		int32_t RenderLayer = m_RenderLayer;
		while (RenderLayer > 0)
		{
			uint32_t CurLayer = (RenderLayer & (-RenderLayer));
			RenderLayer ^= CurLayer;
			auto CurLayerMaterials = GetPassMaterials(CurLayer);
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				CurLayerMaterials[i]->SetParameter(ParameterName, ShaderDataType, ValuePtr);
			}
		}
	}

	Ref<FPipelineStateObject> UMeshRenderComponent::GetPipelineStateObject()
	{
		if (m_PipelineStateObject == nullptr)
		{
			SwitchPso();
		}
		return m_PipelineStateObject;
	}

	void UMeshRenderComponent::SetPsoType(EPsoType PsoType)
	{
		m_Psotype = PsoType;
		SwitchPso();
	}

	void UMeshRenderComponent::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_MaterialHandle")
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

		if (Property->GetPropertyName() == "m_ShaderFile")
		{
			AttachPso();
			UpdateSettings();
			AttachParameters();
		}
		if (Property->GetPropertyName() == "m_Floats" || Property->GetPropertyName() == "m_Textures" || Property->GetPropertyName() == "m_Colors")
		{
			AttachParameters();
		}
	}

	void UMeshRenderComponent::AttachPso()
	{
		if (m_ShaderFile != "")
		{
			Ref<FShader> Shader = TLibrary<FShader>::Fetch(m_ShaderFile);
			CORE_ASSERT(Shader != nullptr, "Shader must be valid!");
			//const FShaderBinderDesc& ShaderBinderDesc = Shader->GetBinder()->GetBinderDesc();
			if (m_PipelineStateObject == nullptr || (Shader != nullptr && GetPipelineStateObject()->GetPSODescriptor().Shader != Shader))
			{
				m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(m_ShaderFile);
			}
		}
		else
		{
			m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PSO_FORWARDLIT);
		}
	
	}

	void UMeshRenderComponent::AttachParameters()
	{
		{
			auto Pso = GetPipelineStateObject();
			int32_t RenderLayer = m_RenderLayer;
			while (RenderLayer > 0)
			{
				uint32_t CurLayer = (RenderLayer & (-RenderLayer));
				RenderLayer ^= CurLayer;
				auto CurLayerMaterials = GetPassMaterials(CurLayer);
				for (size_t i = 0; i < m_SubmeshNum; i++)
				{
					CurLayerMaterials[i]->SetShader(Pso->GetPSODescriptor().Shader);
				}
			}
		}

		if (!m_bEnableMaterial)
		{
			return;
		}
		int32_t RenderLayer = m_RenderLayer;
		if (m_MaterialHandle != "")
		{
			UMaterialAsset* MaterialAsset = FAssetManager::GetInstance().Fetch<UMaterialAsset>(m_MaterialHandle);
			Ref<FShader> Shader = TLibrary<FShader>::Fetch(MaterialAsset->m_ShaderFile);
			if (Shader != nullptr && GetPipelineStateObject()->GetPSODescriptor().Shader != Shader)
			{
				m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(MaterialAsset->m_ShaderFile);
			}
			while (RenderLayer > 0)
			{
				uint32_t CurLayer = (RenderLayer & (-RenderLayer));
				RenderLayer ^= CurLayer;
				auto CurLayerMaterials = GetPassMaterials(CurLayer);

				for (size_t i = 0; i < m_SubmeshNum; i++)
				{
					if (Shader != nullptr)
					{
						CurLayerMaterials[i]->SetShader(Shader);
					}
					for (auto Iter : MaterialAsset->m_Textures)
					{
						const std::string& TextureName = Iter.first;
						Ref<FTexture2D> Texture = FAssetManager::GetInstance().FetchTexture(Iter.second);
						if (Texture != nullptr)
						{
							CurLayerMaterials[i]->SetTexture2D(TextureName, Texture);
						}
					}
					for (auto Iter : MaterialAsset->m_Floats)
					{
						CurLayerMaterials[i]->SetFloat(Iter.first, Iter.second.Value);
					}
				}
			}
		}
		{
			int32_t RenderLayer = m_RenderLayer;
			uint32_t CurLayer = (RenderLayer & (-RenderLayer));
			while (RenderLayer > 0)
			{
				uint32_t CurLayer = (RenderLayer & (-RenderLayer));
				RenderLayer ^= CurLayer;
				auto CurLayerMaterials = GetPassMaterials(CurLayer);

				for (size_t i = 0; i < m_SubmeshNum; i++)
				{
					for (auto Iter : m_Textures)
					{
						if (Iter.second == "")
						{ 
							Iter.second = "default";
						}
						const std::string& TextureName = Iter.first;
						Ref<FTexture2D> Texture = FAssetManager::GetInstance().FetchTexture(Iter.second);
						if (Texture != nullptr)
						{
							CurLayerMaterials[i]->SetTexture2D(TextureName, Texture);
						}
					}
					for (auto Iter : m_Floats)
					{
						CurLayerMaterials[i]->SetFloat(Iter.first, Iter.second.Value);
					}

					for (auto Iter : m_Colors)
					{
						CurLayerMaterials[i]->SetFloat3(Iter.first, Iter.second);
					}

				}
			}
		}
	}
	void UMeshRenderComponent::UpdateSettings()
	{
		const FShaderBinderDesc& ShaderBinderDesc = m_PipelineStateObject->GetPSODescriptor().Shader->GetBinder()->GetBinderDesc();
		{
			UProperty* FloatProperty = GetClassCollection().FindProperty("m_Floats");
			UMapProperty* MapFloatProperty = dynamic_cast<UMapProperty*>(FloatProperty);
			int MaterialBindPoint = ShaderBinderDesc.m_MaterialIndex;
			if (MaterialBindPoint >= 0 && MapFloatProperty != nullptr)
			{
				MapFloatProperty->RemoveAllItem();
				if (ShaderBinderDesc.m_MaterialIndex >= 0)
				{
					const FConstantBufferLayout& Layout = ShaderBinderDesc.GetConstantBufferLayoutByName("cbMaterial");
					const std::vector<FBufferElement>& Elements = Layout.GetElements();
					for (const auto& Element : Elements)
					{
						if (Element.Type == EShaderDataType::Float)
						{
							UProperty* KeyProprety = MapFloatProperty->AddItem();
							UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProprety->Next);
							std::string* KeyPtr = KeyProprety->GetData<std::string>();
							*KeyPtr = Element.Name;
						}
					}
					UpdateEditorContainerPropertyDetails(FloatProperty);
				}
			}
		}
		{
			UProperty* TextureProperty = GetClassCollection().FindProperty("m_Textures");
			UMapProperty* MapTextureProperty = dynamic_cast<UMapProperty*>(TextureProperty);
			if (MapTextureProperty != nullptr)
			{
				MapTextureProperty->RemoveAllItem();
				const FShaderResourceLayout& Layout = ShaderBinderDesc.GetShaderResourceLayout();
				const std::vector<FTextureTableElement>& Elements = Layout.GetElements();
				for (const auto& Element : Elements)
				{
					if (Element.Type == EShaderResourceType::Texture2D)
					{
						UProperty* KeyProprety = MapTextureProperty->AddItem();
						UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProprety->Next);
						std::string* KeyPtr = KeyProprety->GetData<std::string>();
						*KeyPtr = Element.ResourceName;
					}
				}
				UpdateEditorContainerPropertyDetails(TextureProperty);
			}
		}
		{
			UProperty* ColorProperty = GetClassCollection().FindProperty("m_Colors");
			UMapProperty* MapColorProperty = dynamic_cast<UMapProperty*>(ColorProperty);
			if (MapColorProperty != nullptr)
			{
				MapColorProperty->RemoveAllItem();
				if (ShaderBinderDesc.m_MaterialIndex >= 0)
				{
					const FConstantBufferLayout& Layout = ShaderBinderDesc.GetConstantBufferLayoutByName("cbMaterial");
					const std::vector<FBufferElement>& Elements = Layout.GetElements();
					for (const auto& Element : Elements)
					{
						if (Element.Type == EShaderDataType::Float3 || Element.Type == EShaderDataType::RGB)
						{
							UProperty* KeyProprety = MapColorProperty->AddItem();
							UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProprety->Next);
							std::string* KeyPtr = KeyProprety->GetData<std::string>();
							*KeyPtr = Element.Name;
						}
					}
					UpdateEditorContainerPropertyDetails(ColorProperty);
				}
			}
		}
	}

	void UMeshRenderComponent::SwitchPso()
	{
		switch (m_Psotype)
		{
		case Zero::PT_Skybox:
			m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PSO_SKYBOX);
			m_ShaderFile = "Shader\\Skybox.hlsl";
			break;
		case Zero::PT_ForwardLit:
			AttachPso();
			break;
		case Zero::PT_DirectLight:
			m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PSO_DIRECT_LIGHT);
			m_ShaderFile = "Shader\\DirectLight.hlsl";
			break;
		case Zero::PT_PointLight:
			m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PSO_POINT_LIGHT);
			m_ShaderFile = "Shader\\PointLight.hlsl";
			break;
		default:
			break;
		}
	}
}
