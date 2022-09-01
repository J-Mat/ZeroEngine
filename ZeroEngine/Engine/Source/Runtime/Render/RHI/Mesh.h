#pragma once
#include "Core.h"

namespace Zero
{
	struct FSubMesh
	{
		FSubMesh() = default;
		uint32_t Index = 0;
		uint32_t VertexLocation = 0;
		uint32_t IndexLocation = 0;
		uint32_t IndexNumber = 0;
	};
	
	class IVertexBuffer;
	class IIndexBuffer;
	class FMesh
	{
	public:
		FMesh() {};
		using Iter = std::vector<FSubMesh>::iterator;
		Iter begin() { return m_SubMeshes.begin(); }
		Iter end() { return m_SubMeshes.end(); }
		virtual void Draw() = 0;
		virtual void DrawSubMesh(FSubMesh& SubMesh) = 0;
	protected:
		Ref<IVertexBuffer>	m_VertexBuffer;
		Ref<IIndexBuffer>	m_IndexBuffer;
		std::vector<FSubMesh> m_SubMeshes;
	};
}
