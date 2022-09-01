#include "DX12Mesh.h"
#include "Render/RHI/Shader.h"
#include "Render/Moudule/MeshLoader.h"
#include "Render/RHI/VertexBuffer.h"
#include "Render/RHI/IndexBuffer.h"
#include "DX12VertexBuffer.h"
#include "DX12IndexBuffer.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12Mesh::FDX12Mesh(FDX12Device& Device, float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout Layout)
		: FMesh()
		, m_Device(Device)
	{
		m_D3DVertexBuffer = CreateRef<FDX1VertexBuffer>(m_Device, Vertices, VertexCount, Layout);
		m_VertexBuffer = m_D3DVertexBuffer;
		m_D3DIndexBuffer = CreateRef<FDX12IndexBuffer>(m_Device, Indices, IndexCount);
		m_IndexBuffer = m_D3DIndexBuffer;
		FSubMesh SubMesh;
		SubMesh.IndexNumber = IndexCount;
		m_SubMeshes.push_back(SubMesh);
	}
	FDX12Mesh::FDX12Mesh(FDX12Device& Device, const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout Layout)
	: FMesh()
		, m_Device(Device)
	{
		std::vector<float> Vertices;
		std::vector<uint32_t> Indices;
		uint32_t VertexCount = 0;
		uint32_t IndiceCount = 0;
		
		for (size_t Index = 0; Index < MeshDatas.size(); ++Index)
		{
			const FMeshData& MeshData = MeshDatas[Index];
			FSubMesh SubMesh;
			SubMesh.Index = Index;
			SubMesh.VertexLocation = Vertices.size() * sizeof(float) / Layout.GetStride();
			SubMesh.IndexLocation = Indices.size();
			SubMesh.IndexNumber = MeshData.m_Indices.size();
			m_SubMeshes.push_back(SubMesh);
			
			Vertices.insert(Vertices.end(), std::begin(MeshData.m_Vertices), std::end(MeshData.m_Vertices));
			Indices.insert(Indices.end(), std::begin(MeshData.m_Indices), std::end(MeshData.m_Indices));
		}
		
		m_VertexBuffer = CreateRef<FDX1VertexBuffer>(m_Device, Vertices.data(), Vertices.size(), Layout);
		m_IndexBuffer = CreateRef<FDX12IndexBuffer>(m_Device, Indices.data(), Indices.size());
	}
	void FDX12Mesh::Draw()
	{
		Ref<FDX12CommandList> CommandList = m_Device.GetRenderCommandList();
		auto D3DCommandList = CommandList->GetD3D12CommandList();
		D3DCommandList->IASetVertexBuffers(0, 1, &(m_D3DVertexBuffer->GetVertexBufferView()));
		D3DCommandList->IASetIndexBuffer(&(m_D3DIndexBuffer->GetIndexBufferView()));
		D3DCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3DCommandList->DrawIndexedInstanced(m_D3DIndexBuffer->GetIndexCount(), 1, 0, 0, 0);
	}
	void FDX12Mesh::DrawSubMesh(FSubMesh& SubMesh)
	{
		Ref<FDX12CommandList> CommandList = m_Device.GetRenderCommandList();
		auto D3DCommandList = CommandList->GetD3D12CommandList();
		D3DCommandList->IASetVertexBuffers(0, 1, &(m_D3DVertexBuffer->GetVertexBufferView()));
		D3DCommandList->IASetIndexBuffer(&(m_D3DIndexBuffer->GetIndexBufferView()));
		D3DCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3DCommandList->DrawIndexedInstanced(SubMesh.IndexNumber, 1, SubMesh.IndexLocation, SubMesh.VertexLocation, 0);
	}
}