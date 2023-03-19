#pragma once

#include "Core.h"
#include "../DX12/Common/DX12Header.h"
#include "Render/RHI/Mesh.h"


namespace Zero
{
	class FDX12Device;
	struct FMeshData;
	class FVertexBufferLayout;
	class FDX12Buffer;
	class FDX12Mesh : public FMesh
	{
	public:
		FDX12Mesh(float* Vertices, uint32_t VertexCount, uint32_t* indices, uint32_t IndexCount, FVertexBufferLayout& Layout);
		FDX12Mesh(std::vector<FMeshData> meshDatas, FVertexBufferLayout& Layout);
		virtual void Draw(FCommandListHandle CommandListHandle);
		virtual void DrawSubMesh(FSubMesh& SubMesh, FCommandListHandle CommandListHandle);
	private:
		Ref<FDX12Buffer> m_VertexBuffer;
		Ref<FDX12Buffer> m_IndexBuffer;

		D3D12_VERTEX_BUFFER_VIEW m_VBView;
		D3D12_INDEX_BUFFER_VIEW m_IBView;
	};
}
