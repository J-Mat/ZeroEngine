#pragma once

#include "Core.h"
#include "MeshActor.h"
#include "CubeMeshActor.reflection.h"

namespace Zero
{
	UCLASS()
	class UCubeMeshActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		UCubeMeshActor();
		virtual void BuildMesh();
	private:
		float m_Width = 2.0f;
		float m_Height = 2.0f;
		float m_Depth = 2.0f;
	};
}