#include "MeshActor.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"

namespace Zero
{
	UMeshActor::UMeshActor(const std::string& Tag)
		: UActor(Tag)
	{
		m_MeshVertexComponent = CreateObject<UMeshVertexComponent>(GetWorld());
		m_MeshRenderComponent = CreateObject<UMeshRenderComponent>(GetWorld());
	}
}