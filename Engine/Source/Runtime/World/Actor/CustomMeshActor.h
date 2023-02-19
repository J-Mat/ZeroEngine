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
		UCustomMeshActor();
		UCustomMeshActor(const std::string& FileName);
		virtual void BuildMesh();
	protected:
		UPROPERTY()
		std::string m_FileName;
	};
}