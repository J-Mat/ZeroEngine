#pragma once

#include "Core.h"
#include "Actor.h"
#include "World/Component/CameraComponent.h"

namespace Zero
{
	class IShaderConstantsBuffer;
	class UCameraActor : public UActor
	{
	public:
		UCameraActor(const std::string& Tag = "Camera");
		virtual void PostInit();
		virtual void Tick();
		virtual void SetRotation(const ZMath::vec3& Rotation);
		Ref<IShaderConstantsBuffer> GetConstantBuffer() { return m_CameraComponent->GetConstantBuffer(); }
		UCameraComponent* m_CameraComponent = nullptr;
	};
}