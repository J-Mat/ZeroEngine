#pragma once
#include "Core.h"

namespace Zero
{
	struct FSubMesh
	{
		FSubMesh() = default;
		uint32_t Index = -1;
		uint32_t VertexLocation = 0;
		uint32_t IndexLocation = 0;
		uint32_t IndexNumber = 0;
		bool IsNull() { return Index == -1; }
	};
	
	class FVertexBuffer;
	class FIndexBuffer;
	struct FCommandListHandle;
	class FMesh
	{
	public:
		FMesh() {};
		using Iter = std::vector<FSubMesh>::iterator;
		Iter begin() { return m_SubMeshes.begin(); }
		Iter end() { return m_SubMeshes.end(); }
		virtual void Draw(FCommandListHandle CommandListHandle) = 0;
		virtual void DrawSubMesh(FSubMesh& SubMesh, FCommandListHandle CommandListHandle) = 0;
		uint32_t GetSubMeshNum() { return uint32_t(m_SubMeshes.size()); }
		ZMath::FAABB& GetAABB() { return AABB; };
		void Merge(ZMath::FAABB& Other) { AABB.Merge(Other); };
	protected:
		//Ref<FVertexBuffer>	m_VertexBuffer;
		//Ref<FIndexBuffer>	m_IndexBuffer;
		std::vector<FSubMesh> m_SubMeshes;
		ZMath::FAABB AABB;
	};
}
