#pragma once
#include "World/World.h"
#include "G:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\CameraActor.h"
#include "G:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\CustomMeshActor.h"
#include "G:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\MeshActor.h"


namespace Zero
{
	static UActor* CreateActorByName(UWorld* World, const std::string& ClassName)
	{
		if (ClassName == "UCameraActor")
		{
			return World->CreateActorRaw<UCameraActor>();
		}
		else if (ClassName == "UCustomMeshActor")
		{
			return World->CreateActorRaw<UCustomMeshActor>();
		}
		else if (ClassName == "UMeshActor")
		{
			return World->CreateActorRaw<UMeshActor>();
		}
		return nullptr;
	}
}
