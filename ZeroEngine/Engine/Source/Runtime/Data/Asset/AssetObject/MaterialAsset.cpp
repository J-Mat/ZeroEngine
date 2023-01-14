#include "MaterialAsset.h"
#include "World/Object/PropertyObject.h"
#include "World/Object/MapPropretyObject.h"
#include "Render/RHI/Shader.h"
#include "Core/Framework/Library.h"


namespace Zero
{
	void UMaterialAsset::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_ShaderFile")
		{
			Ref<FShader> Shader = TLibrary<FShader>::Fetch(m_ShaderFile);
			CORE_ASSERT(Shader != nullptr, "Shader must be valid!");
			const FShaderBinderDesc& ShaderBinderDesc = Shader->GetBinder()->GetBinderDesc();
			
			{
				UProperty* FloatProperty = GetClassCollection().FindProperty("m_Floats");
				UMapProperty* MapFloatProperty = dynamic_cast<UMapProperty*>(FloatProperty);
				int MaterialBindPoint = ShaderBinderDesc.m_MaterialIndex;
				if (MaterialBindPoint >= 0 && MapFloatProperty != nullptr)
				{
					MapFloatProperty->RemoveAllItem();
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

			{
				UProperty* TextureProperty = GetClassCollection().FindProperty("m_Textures");
				UMapProperty* MapTextureProperty = dynamic_cast<UMapProperty*>(TextureProperty);
				if (MapTextureProperty != nullptr)
				{
					MapTextureProperty->RemoveAllItem();
					const FShaderResourceLayout&  Layout = ShaderBinderDesc.GetTextureResourceLayout();
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
		}
	}
}
