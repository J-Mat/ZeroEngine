#include "MeshRenderComponent.h"
#include "Render/RHI/Shader/ShaderData.h"
#include "World/World.h"
#include "Render/Moudule/ShaderParamsSettings.h"
#include "Data/Asset/AssetManager.h"
#include "World/Object/PropertyObject.h"
#include "World/Object/MapPropretyObject.h"
#include "Data/Asset/AssetObject/MaterialAsset.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
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
		UpdateSettings();
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

	std::vector<Ref<FShaderParamsSettings>>& UMeshRenderComponent::GetPassMaterials(ERenderLayer RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		return Iter->second->ShaderParamsSettings;
	}

	void UMeshRenderComponent::SetSubmeshNum(uint32_t Num)
	{
		m_SubmeshNum = Num;
		for (auto& Iter : m_LayerInfo)
		{
			ERenderLayer RenderLayer = Iter.first;
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				Ref<FShaderParamsSettings> Material = CreateRef<FShaderParamsSettings>();
				//Material->SetShader(Iter.second->PipelineStateObject->GetPSODescriptor().Shader);
				Iter.second->ShaderParamsSettings.push_back(Material);
			}
		}
		
	}

	void UMeshRenderComponent::AttachRenderLayer(ERenderLayer RenderLayer, uint32_t PsoID)
	{
		m_RenderLayer |= (uint32_t)RenderLayer;
		Ref<FRenderLayerInfo> RenderLayerInfo = CreateRef<FRenderLayerInfo>();
		RenderLayerInfo->PsoID = PsoID;
		m_LayerInfo.insert({ RenderLayer, RenderLayerInfo });
		FPSOCache::Get().GetPsoRecreateEvent().AddFunction<UMeshRenderComponent>(this, &UMeshRenderComponent::OnReCreateGraphicPSO);
	}

	void UMeshRenderComponent::SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr, ERenderLayer RenderLayer)
	{
		auto& Iter = m_LayerInfo.find(RenderLayer);
		if (Iter != m_LayerInfo.end())
		{
			auto& CurLayerMaterials = Iter->second->ShaderParamsSettings;
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

	void UMeshRenderComponent::OnReCreateGraphicPSO(uint32_t PsoID)
	{
		for (auto& [_, RenderLayerInfo] : m_LayerInfo)
		{
			if (PsoID == RenderLayerInfo->PsoID)
			{ 
				for (auto Material : RenderLayerInfo->ShaderParamsSettings)
				{
					auto Pso = FPSOCache::Get().FetchGraphicPso(PsoID);
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

		if (Property->GetPropertyName() == "m_Floats" || Property->GetPropertyName() == "m_TextureHandles" || Property->GetPropertyName() == "m_Colors")
		{
			m_Textures.clear();
			m_bUpdateIfDirty = true;
		}
		if (Property->GetPropertyName() == "m_ShadingMode")
		{
			m_PerObjConstantsBuffer->SetInt("ShadingMode", m_ShadingMode);
			m_bUpdateIfDirty = true;
		}
	}

	void UMeshRenderComponent::Tick()
	{
		Supper::Tick();
		AttachParametersToShader();
	}

	void UMeshRenderComponent::AttachParametersToShader()
	{
		if (m_bUpdateIfDirty)
		{
			for (auto Iter : m_TextureHandles)
			{ 
				m_Textures.insert(std::make_pair(Iter.first, FTextureManager::Get().GetTextureByHandle(Iter.second)));
			}
			m_bUpdateIfDirty = false;
		}

		for (auto& RenderLayer : m_LayerInfo)
		{
			if (RenderLayer.first != ERenderLayer::Opaque && RenderLayer.first != ERenderLayer::Transparent)
			{
				continue;
			}
			for (size_t i = 0; i < m_SubmeshNum; i++)
			{
				for (auto Iter : m_Textures)
				{
					const std::string& TextureName = Iter.first;
					Ref<FTexture2D> Texture = Iter.second;
					if (Texture != nullptr)
					{
						RenderLayer.second->ShaderParamsSettings[i]->SetTexture2D(TextureName, Texture.get());
					}
				}
				for (auto Iter : m_Floats)
				{
					RenderLayer.second->ShaderParamsSettings[i]->SetFloat(Iter.first, Iter.second.Value);
				}

				for (auto Iter : m_Colors)
				{
					RenderLayer.second->ShaderParamsSettings[i]->SetFloat3(Iter.first, Iter.second);
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
		const FShaderBinderDesc& ShaderBinderDesc = FPSOCache::Get().FetchGraphicPso(PsoID)->GetPSODescriptor().Shader->GetBinder()->GetBinderDesc();
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
			UProperty* TextureProperty = GetClassCollection().FindProperty("m_TextureHandles");
			UMapProperty* MapTextureProperty = dynamic_cast<UMapProperty*>(TextureProperty);
			if (MapTextureProperty != nullptr)
			{
				MapTextureProperty->RemoveAllItem();
				const FSRVResourceLayout& Layout = ShaderBinderDesc.GetTextureResourceLayout();
				const std::vector<FSRVElement>& Elements = Layout.GetElements();
				for (const auto& Element : Elements)
				{
					if (Element.Type == EShaderResourceType::Texture2D && !Element.ResourceName.starts_with("_"))
					{
						UProperty* KeyProprety = MapTextureProperty->AddItem();
						UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProprety->Next);
						std::string* KeyPtr = KeyProprety->GetData<std::string>();
						*KeyPtr = Element.ResourceName;
						if (auto Iter = m_TextureHandles.find(Element.ResourceName); Iter != m_TextureHandles.end())
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
		m_bUpdateIfDirty = true;
	}
}

