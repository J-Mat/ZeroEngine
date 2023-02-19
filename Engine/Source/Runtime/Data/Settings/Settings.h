#pragma once

#include "Core.h"
#include "Data/SerializeUtility.h"
#include "World/Object/CoreObject.h"
#include "Settings.reflection.h"

namespace Zero
{
	UCLASS()
	class USettings : public UCoreObject
	{
		GENERATED_BODY()
	public:
		USettings() = default;
	};

}