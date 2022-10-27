#pragma once 
#include "Core.h"
#include "Actor.h"
#include "World/Component/CameraComponent.h"
#include "CameraActor.reflection.h"

namespace Zero
{
	class IShaderConstantsBuffer;
	UCLASS()
	class UCameraActor : public UActor
	{
		GENERATED_BODY()
	public:
		UCameraActor();
		virtual void Tick();
		virtual void SetRotation(const ZMath::vec3& Rotation);
		Ref<IShaderConstantsBuffer> GetConstantBuffer() { return m_CameraComponent->GetConstantBuffer(); }
		void OnResizeViewport(uint32_t Width, uint32_t Height);

		template<class UCameraComponent>
		UCameraComponent* GetComponent() 
		{
			return m_CameraComponent;
		}
	private:
		UPROPERTY()
		UCameraComponent* m_CameraComponent = nullptr;
	};
}