#pragma once

#include "Core.h"
#include "MeshActor.h"

namespace Zero
{
	class UCubeMeshActor : public UMeshActor
	{
	public:
		UCubeMeshActor();
		virtual void BuildMesh();
	private:
		float m_Width = 2.0f;
		float m_Height = 2.0f;
		float m_Depth = 2.0f;
	};
}