#pragma once

#include "Component.h"
#include "Render/RHI/Mesh.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/Moudule/MeshGenerator.h"
#include "MeshVertexComponent.reflection.h"


namespace Zero
{
	class FMesh;
	UCLASS()
	class UMeshVertexComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		friend class UMeshActor;
		friend class USkyActor;
		UMeshVertexComponent();
		virtual ~UMeshVertexComponent();
		std::vector<FMeshData>& GetMeshDatas() { return m_MeshDatas; }
		void CreateMesh();
		void GenerateAABB();
	protected:
		Ref<FMesh> m_Mesh;
		FMeshType m_MeshType;
		std::vector<FMeshData> m_MeshDatas;
	};
}