#pragma once

#include "Component.h"
#include "Render/RHI/Mesh.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/Moudule/MeshGenerator.h"
#include "MeshVertexComponent.reflection.h"


namespace Zero
{
	class FPerObjectConstantsBufferPool : public IPublicSingleton<FPerObjectConstantsBufferPool>
	{
	public:
		Ref<IShaderConstantsBuffer> GetPerObjectConstantsBuffer(UCoreObject* Obj);
		void PushToPool(Ref<IShaderConstantsBuffer> Buffer);
	private:
		std::queue<Ref<IShaderConstantsBuffer>> m_IdleConstantsBuffer;
	};
	class FMesh;
	UCLASS()
	class UMeshVertexComponent : public UComponent
	{
		GENERATED_BODY()
	public:
		friend class UMeshActor;
		UMeshVertexComponent();
		virtual ~UMeshVertexComponent();
		std::vector<FMeshData>& GetMeshDatas() { return m_MeshDatas; }
		void CreateMesh();
		void GenerateAABB();
	protected:
		Ref<IShaderConstantsBuffer> m_ShaderConstantsBuffer = nullptr;
		Ref<FMesh> m_Mesh;
		FMeshType m_MeshType;
		std::vector<FMeshData> m_MeshDatas;
	};
}