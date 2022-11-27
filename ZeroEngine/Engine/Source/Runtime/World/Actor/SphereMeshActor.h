#pragma once

#include "Core.h"
#include "MeshActor.h"
#include "SphereMeshActor.reflection.h"

namespace Zero
{
	UCLASS()
	class USphereMeshActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		USphereMeshActor();
		USphereMeshActor(float Radius, uint32_t NumSubdivisions);
		virtual void BuildMesh();
	private:
		float m_Radius = 1.0f;
		uint32_t m_NumSubdivisions = 3;
	};
}