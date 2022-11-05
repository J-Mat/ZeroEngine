#pragma once
#include "World/World.h"
#include "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\CameraActor.h"
#include "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\CubeMeshActor.h"
#include "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\CustomMeshActor.h"
#include "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\LightActor.h"
#include "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\MeshActor.h"
#include "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source\Runtime/World\Actor\SphereMeshActor.h"


namespace Zero
{
	static UActor* CreateActorByName(UWorld* World, const std::string& ClassName)
	{
		if (ClassName == "UCameraActor")
		{
			return World->CreateActorRaw<UCameraActor>();
		}
		else if (ClassName == "UCubeMeshActor")
		{
			return World->CreateActorRaw<UCubeMeshActor>();
		}
		else if (ClassName == "UCustomMeshActor")
		{
			return World->CreateActorRaw<UCustomMeshActor>();
		}
		else if (ClassName == "ULightActor")
		{
			return World->CreateActorRaw<ULightActor>();
		}
		else if (ClassName == "UMeshActor")
		{
			return World->CreateActorRaw<UMeshActor>();
		}
		else if (ClassName == "USphereMeshActor")
		{
			return World->CreateActorRaw<USphereMeshActor>();
		}
		return nullptr;
	}
}
