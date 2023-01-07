#include "MeshRenderComponent.h"
#include "Render/RHI/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"
#include "Data/Asset/AssetManager.h"
#include "World/Object/PropertyObject.h"
#include "World/Object/MapPropretyObject.h"
#include "Data/Asset/AssetObject/MaterialAsset.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{
	UMeshRenderComponent::UMeshRenderComponent()
		: UComponent()
	{
		m_PerObjConstantsBuffer = FConstantsBufferManager::GetInstance().GetPerObjectConstantsBuffer();
	}

	UMeshRenderComponent::~UMeshRenderComponent()
	{
		FConstantsBufferManager::GetInstance().PushPerObjectConstantsBufferToPool(m_PerObjConstantsBuffer);
	}

	void UMeshRenderComponent::PostInit()
	{
		m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
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

	std::vector<Ref<FMaterial>>& UMeshRenderComponent::GetPassMaterials(uint32_t RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		return Iter->second->Materials;
	}

	void UMeshRenderComponent::SetSubmeshNum(uint32_t Num)
	{
		m_SubmeshNum = Num;
		for (auto& Iter : m_LayerInfo)
		{
			uint32_t RenderLayer = Iter.first;
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				Ref<FMaterial> Material;
				if (RenderLayer == RENDERLAYER_SHADOW)
				{
					Material = CreateRef<FMaterial>(false);
				}
				else
				{
					Material = CreateRef<FMaterial>();
				}
				Material->SetShader(Iter.second->PipelineStateObject->GetPSODescriptor().Shader);
				Iter.second->Materials.push_back(Material);
			}
		}
		
	}

	void UMeshRenderComponent::AttachRenderLayer(int32_t RenderLayer, const std::string& PsoType)
	{
		m_RenderLayer |= RenderLayer;
		Ref<FRenderLayerInfo> RenderLayerInfo = CreateRef<FRenderLayerInfo>();
		RenderLayerInfo->PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch(PsoType);
		m_LayerInfo.insert({ RenderLayer, RenderLayerInfo });
	}

	void UMeshRenderComponent::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, uint32_t RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		if (Iter != m_LayerInfo.end())
		{
			auto& CurLayerMaterials = Iter->second->Materials;
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				CurLayerMaterials[i]->SetParameter(ParameterName, ShaderDataType, ValuePtr);
			}
		}
	}

	Ref<FPipelineStateObject> UMeshRenderComponent::GetPipelineStateObject(uint32_t RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		return Iter->second->PipelineStateObject;
	}

	void UMeshRenderComponent::SetShadingMode(EShadingMode ShadingMode)
	{
		m_ShadingMode = ShadingMode;
		m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
	}

	void UMeshRenderComponent::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_MaterialHandle")
		{
			AttachParameters();
		}

		if (Property->GetPropertyName() == "m_Floats" || Property->GetPropertyName() == "m_Textures" || Property->GetPropertyName() == "m_Colors")
		{
			AttachParameters();
		}
		if (Property->GetPropertyName() == "m_ShadingMode")
		{
			m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
		}
	}

	void UMeshRenderComponent::AttachParameters()
	{
		if (!m_bEnableMaterial)
		{
			return;
		}
		if (m_MaterialHandle != "")
		{
			/*
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
			*/
		}
		{
			auto& OpaqueLayer = m_LayerInfo.find(RENDERLAYER_OPAQUE);

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
						OpaqueLayer->second->Materials[i]->SetTexture2D(TextureName, Texture);
					}
				}
				for (auto Iter : m_Floats)
				{
					OpaqueLayer->second->Materials[i]->SetFloat(Iter.first, Iter.second.Value);
				}

				for (auto Iter : m_Colors)
				{
					OpaqueLayer->second->Materials[i]->SetFloat3(Iter.first, Iter.second);
				}

			}
		}
	}
	void UMeshRenderComponent::UpdateSettings()
	{
		auto& OpaqueLayer = m_LayerInfo.find(RENDERLAYER_OPAQUE);
		if (OpaqueLayer == m_LayerInfo.end())
		{
			return;
		}

		const FShaderBinderDesc& ShaderBinderDesc = GetPipelineStateObject(RENDERLAYER_OPAQUE)->GetPSODescriptor().Shader->GetBinder()->GetBinderDesc();
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
					if (Element.Type == EShaderResourceType::Texture2D && !Element.ResourceName.starts_with("_"))
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
}

