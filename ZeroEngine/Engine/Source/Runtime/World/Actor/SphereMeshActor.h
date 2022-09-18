#pragma once

#include "Core.h"
#include "MeshActor.h"

namespace Zero
{
	class USphereMeshActor : public UMeshActor
	{
	public:
		USphereMeshActor(const std::string& Tag = "Sphere");
		virtual void BuildMesh();
	private:
		float m_Radius = 2.0f;
	};
}