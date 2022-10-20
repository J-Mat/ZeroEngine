#pragma once

#include "Core.h"
#include "MeshActor.h"
#include "CustomMeshActor.reflection.h"

namespace Zero
{
	UCLASS()
	class UCustomMeshActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		UCustomMeshActor(const std::string& FileName = "cat.fbx");
		virtual void BuildMesh();
	protected:
		std::string m_FileName;
	};
}