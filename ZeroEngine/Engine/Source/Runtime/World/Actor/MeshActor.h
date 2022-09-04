#pragma once

#include "Core.h"
#include "Actor.h"

namespace Zero
{
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class UMeshActor : public UActor
	{
	public:
		UMeshActor(uint32_t DeviceIndex, const std::string& Tag);
	protected:
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		UMeshRenderComponent* m_MeshRenderComponent = nullptr;
	};
}