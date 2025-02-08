#pragma once

#include "Core.h"
#include "../Object/CoreObject.h"
#include "../Object/PropertyObject.h"
#include "Material.reflection.h"

namespace Zero
{
	class UCoreObject;
	UCLASS()
	class UMaterial : public UCoreObject
	{
		GENERATED_BODY()
	public:	
		UMaterial();	

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, FTextureHandle> m_TextureHandles;

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, FFloatSlider> m_Floats;

		UPROPERTY(Fixed, KeyUnEditable)
		std::map<std::string, ZMath::FColor3> m_Colors;
	};
}