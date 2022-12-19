#pragma once
#include "../../Object/ObjectMacros.h"
#include "../Component.h"
#include "../TransformComponent.h"
#include "LightComponent.reflection.h"

namespace Zero
{
	UCLASS()
	class ULightComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		ULightComponent() = default;
		virtual void PostInit();
		
		const ZMath::FColor3& GetLightColor() const { return m_LightColor; }
		float GetLightIntensity() { return m_LightIntensity; }
		
	protected:
		UPROPERTY()
		ZMath::FColor3 m_LightColor = {1.0f, 1.0f, 1.0f};

		UPROPERTY()
		float m_LightIntensity = 300.0f;	

		UTransformComponent* m_TransformCompent;
	};
}