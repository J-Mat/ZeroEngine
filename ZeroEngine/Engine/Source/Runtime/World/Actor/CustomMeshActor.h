#pragma once

#include "Core.h"
#include "MeshActor.h"

namespace Zero
{
	class UCustomMeshActor : public UMeshActor
	{
	public:
		UCustomMeshActor(const std::string& Tag = "Custom", const std::string& FileName = "cat.fbx");
		virtual void BuildMesh();
	private:
		std::string m_FileName = "";
	};
}