#pragma once

#include "Component.h"
#include "Render/RHI/Mesh.h"
#include "Render/RHI/ShaderBinder.h"
#include "MeshVertexComponent.h"


namespace Zero
{
	class UCubeMeshVertexComponent : public UMeshVertexComponent
	{
	public:
		friend class UMeshActor;
		UCubeMeshVertexComponent();
		virtual ~UCubeMeshVertexComponent() = default;
	};
}