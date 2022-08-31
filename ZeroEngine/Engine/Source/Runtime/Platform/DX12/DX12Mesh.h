#pragma once

#include "Core.h"
#include "../DX12/Common/DX12Header.h"
#include "Render/RHI/Mesh.h"


namespace Zero
{
	class FDX12Device;
	struct FMeshData;
	class FVertexBufferLayout;
	class FDX12Mesh : public FMesh
	{
	public:
		FDX12Mesh(FDX12Device& Device, float* Vertices, uint32_t VertexCount, uint32_t* indices, uint32_t IndexCount, FVertexBufferLayout Layout);
		FDX12Mesh(FDX12Device& Device, const std::vector<FMeshData>& meshDatas, FVertexBufferLayout Layout);
		virtual Draw(uint32_t Slot);
	private:
		FDX12Device& m_Device;
	};
}
