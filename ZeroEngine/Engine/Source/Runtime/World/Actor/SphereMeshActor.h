#pragma once

#include "Core.h"
#include "MeshActor.h"

namespace Zero
{
	class USphereMeshActor : public UMeshActor
	{
	public:
		USphereMeshActor();
		USphereMeshActor(float Radius, uint32_t NumSubdivisions);
		virtual void BuildMesh();
	private:
		float m_Radius = 2.0f;
		uint32_t m_NumSubdivisions = 3;
	};
}