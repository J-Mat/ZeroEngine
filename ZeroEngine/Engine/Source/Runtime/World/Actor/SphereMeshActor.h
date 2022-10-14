#pragma once

#include "Core.h"
#include "MeshActor.h"

namespace Zero
{
	class USphereMeshActor : public UMeshActor
	{
	public:
		USphereMeshActor();
		virtual void BuildMesh();
	private:
		float m_Radius = 2.0f;
	};
}