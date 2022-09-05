#pragma once

#include "Core.h"
#include "Actor.h"

namespace Zero
{
	class UCameraComponent;
	class UCameraActor : public UActor
	{
	public:
		UCameraActor(const std::string& Tag);
	protected:
		UCameraComponent* CaemraComponent = nullptr;
	};
}