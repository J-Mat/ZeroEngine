#include "MeshRenderComponent.h"
#include "Render/RHI/Shader/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/Material.h"
#include "Data/Asset/AssetManager.h"
#include "World/Object/PropertyObject.h"
#include "World/Object/MapPropretyObject.h"
#include "Data/Asset/AssetObject/MaterialAsset.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/Moudule/ConstantsBufferManager.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "Render/Moudule/PSOCache.h"

namespace Zero
{
	UMeshRenderComponent::UMeshRenderComponent()
		: UComponent()
	{
		m_PerObjConstantsBuffer = FConstantsBufferManager::Get().GetPerObjectConstantsBuffer();
	}

	UMeshRenderComponent::~UMeshRenderComponent()
	{
		FConstantsBufferManager::Get().PushPerObjectConstantsBufferToPool(m_PerObjConstantsBuffer);
	}

	void UMeshRenderComponent::PostInit()
	{
		//m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
	}

	void UMeshRenderComponent::SetEnableMaterial(bool bEnable)
	{
		m_bEnableMaterial = bEnable;
		if (m_bEnableMaterial)
		{
			m_ClassInfoCollection.RemoveField("m_MaterialHandle", "Invisible");
			m_ClassInfoCollection.RemoveField("m_ShadingMode", "Invisible");
		}
		else
		{
			m_ClassInfoCollection.AddField("m_MaterialHandle", "Invisible");
			m_ClassInfoCollection.AddField("m_ShadingMode", "Invisible");
		}
	}

	std::vector<Ref<FMaterial>>& UMeshRenderComponent::GetPassMaterials(ERenderLayer RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		return Iter->second->Materials;
	}

	void UMeshRenderComponent::SetSubmeshNum(uint32_t Num)
	{
		m_SubmeshNum = Num;
		for (auto& Iter : m_LayerInfo)
		{
			ERenderLayer RenderLayer = Iter.first;
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				Ref<FMaterial> Material;
				if (RenderLayer == ERenderLayer::Shadow)
				{
					Material = CreateRef<FMaterial>(false);
				}
				else
				{
					Material = CreateRef<FMaterial>();
				}
				//Material->SetShader(Iter.second->PipelineStateObject->GetPSODescriptor().Shader);
				Iter.second->Materials.push_back(Material);
			}
		}
		
	}

	void UMeshRenderComponent::AttachRenderLayer(ERenderLayer RenderLayer, uint32_t PsoID)
	{
		m_RenderLayer |= (uint32_t)RenderLayer;
		Ref<FRenderLayerInfo> RenderLayerInfo = CreateRef<FRenderLayerInfo>();
		RenderLayerInfo->PsoID = PsoID;
		m_LayerInfo.insert({ RenderLayer, RenderLayerInfo });
		FPSOCache::Get().GetPsoRecreateEvent().AddFunction<UMeshRenderComponent>(this, &UMeshRenderComponent::OnRecreatePso);
	}

	void UMeshRenderComponent::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, ERenderLayer RenderLayer)
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

	uint32_t UMeshRenderComponent::GetPsoID(ERenderLayer RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		return Iter->second->PsoID;
	}

	void UMeshRenderComponent::OnRecreatePso(uint32_t PsoID)
	{
		for (auto& [_, RenderLayerInfo] : m_LayerInfo)
		{
			if (PsoID == RenderLayerInfo->PsoID)
			{ 
				for (auto Material : RenderLayerInfo->Materials)
				{
					auto Pso = FPSOCache::Get().Fetch(PsoID);
					Material->SetShader(Pso->GetPSODescriptor().Shader);
				}
				UpdateSettings();
				AttachParametersToShader();
			}
		}
	}

	void UMeshRenderComponent::SetShadingMode(EShadingMode ShadingMode)
	{
		m_ShadingMode = ShadingMode;
		m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
	}

	Ref<IShaderConstantsBuffer> UMeshRenderComponent::GetAndPreDraCallPerObjConstanBuffer()
	{
		m_PerObjConstantsBuffer->PreDrawCall();
		m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
		return m_PerObjConstantsBuffer;
	}

	void UMeshRenderComponent::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_MaterialHandle")
		{
			AttachParametersToShader();
		}

		if (Property->GetPropertyName() == "m_Floats" || Property->GetPropertyName() == "m_Textures" || Property->GetPropertyName() == "m_Colors")
		{
			AttachParametersToShader();
		}
		if (Property->GetPropertyName() == "m_ShadingMode")
		{
			m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
			AttachParametersToShader();
		}
	}

	void UMeshRenderComponent::AttachParametersToShader()
	{
		if (!m_bEnableMaterial)
		{
			return;
		}
		if (m_MaterialHandle != "")
		{
		}
		{
			auto& OpaqueLayer = m_LayerInfo.find(ERenderLayer::Opaque);

			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				for (auto Iter : m_Textures)
				{
					if (Iter.second.IsNull())
					{
						Iter.second = FTextureManager::Get().GetDefaultTextureHandle();
					}
					const std::string& TextureName = Iter.first;
					Ref<FTexture2D> Texture = FTextureManager::Get().GetTextureByHandle(Iter.second);
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
		auto& OpaqueLayer = m_LayerInfo.find(ERenderLayer::Opaque);
		if (OpaqueLayer == m_LayerInfo.end())
		{
			return;
		}

		uint32_t PsoID = GetPsoID(ERenderLayer::Opaque);
		const FShaderBinderDesc& ShaderBinderDesc = FPSOCache::Get().Fetch(PsoID)->GetPSODescriptor().Shader->GetBinder()->GetBinderDesc();
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
				const FShaderResourceLayout& Layout = ShaderBinderDesc.GetTextureResourceLayout();
				const std::vector<FTextureTableElement>& Elements = Layout.GetElements();
				for (const auto& Element : Elements)
				{
					if (Element.Type == EShaderResourceType::Texture2D && !Element.ResourceName.starts_with("_"))
					{
						UProperty* KeyProprety = MapTextureProperty->AddItem();
						UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProprety->Next);
						std::string* KeyPtr = KeyProprety->GetData<std::string>();
						*KeyPtr = Element.ResourceName;
						if (auto Iter = m_Textures.find(Element.ResourceName); Iter != m_Textures.end())
						{
							FTextureHandle Handle = Iter->second;
							FTextureHandle* ValuePtr = ValueProperty->GetData<FTextureHandle>();
							*ValuePtr = Handle;
						}
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

