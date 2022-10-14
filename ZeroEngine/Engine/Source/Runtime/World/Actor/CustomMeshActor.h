#pragma once

#include "Core.h"
#include "MeshActor.h"

namespace Zero
{
	class UCustomMeshActor : public UMeshActor
	{
	public:
		UCustomMeshActor(const std::string& FileName = "cat.fbx");
		virtual void BuildMesh();
	protected:
		std::string m_FileName;
	};
}