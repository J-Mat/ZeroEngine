#pragma once

#include "Core.h"
#include "Actor.h"

namespace Zero
{
	class UCameraComponent;
	class UCameraActor : public UActor
	{
	public:
		UCameraActor(const std::string& Tag = "Camera");
		virtual void PostInit();
		virtual void Tick();
	protected:
		UCameraComponent* m_CameraComponent = nullptr;
	};
}