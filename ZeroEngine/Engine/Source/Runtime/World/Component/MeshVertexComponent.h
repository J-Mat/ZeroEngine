#pragma once

#include "Component.h"
#include "Render/RHI/Mesh.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/Moudule/MeshGenerator.h"


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
	class UMeshVertexComponent : public UComponent
	{
	public:
		friend class UMeshActor;
		UMeshVertexComponent(FMeshType& MeshType);
		virtual void PostInit();
		virtual ~UMeshVertexComponent();
	protected:
		Ref<IShaderConstantsBuffer> m_ShaderConstantsBuffer = nullptr;
		Ref<FMesh> m_Mesh;
		FMeshType m_MeshType;
		FMeshData m_MeshData;
	};
}