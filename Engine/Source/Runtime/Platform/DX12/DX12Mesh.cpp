#include "DX12Mesh.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/Moudule/MeshGenerator.h"
#include "DX12CommandList.h"
#include "DX12Buffer.h"

namespace Zero
{
	FDX12Mesh::FDX12Mesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout)
		: FMesh()
	{
		auto VectexBufferDesc = FBufferDesc::VertexBufferDesc(VertexCount, Layout.GetStride());
		//m_D3DVertexBuffer = CreateRef<FDX12VertexBuffer>(VectexBufferDesc, Vertices,  Layout);
		//m_VertexBuffer = m_D3DVertexBuffer;

		auto IndexBufferDesc = FBufferDesc::IndexBufferDesc(IndexCount);
		//m_D3DIndexBuffer = CreateRef<FDX12IndexBuffer>(IndexBufferDesc, Indices);
		//m_IndexBuffer = m_D3DIndexBuffer;
		FSubMesh SubMesh;
		SubMesh.Index = 0;
		SubMesh.IndexNumber = IndexCount;
		m_SubMeshes.push_back(SubMesh);
	}

	FDX12Mesh::FDX12Mesh(std::vector<FMeshData> MeshDatas, FVertexBufferLayout& Layout)
	: FMesh()
	{
		std::vector<float> Vertices;
		std::vector<uint32_t> Indices;
		uint32_t VertexCount = 0;
		uint32_t IndiceCount = 0;
		
		for (uint32_t Index = 0; Index < MeshDatas.size(); ++Index)
		{
			const FMeshData& MeshData = MeshDatas[Index];
			FSubMesh SubMesh;
			SubMesh.Index = Index;
			SubMesh.VertexLocation = uint32_t(Vertices.size()) * sizeof(float) / Layout.GetStride();
			SubMesh.IndexLocation = uint32_t(Indices.size());
			SubMesh.IndexNumber = uint32_t(MeshData.Indices.size());
			m_SubMeshes.push_back(SubMesh);
			
			Vertices.insert(Vertices.end(), std::begin(MeshData.Vertices), std::end(MeshData.Vertices));
			Indices.insert(Indices.end(), std::begin(MeshData.Indices), std::end(MeshData.Indices));
		}
		VertexCount = uint32_t(Vertices.size() * sizeof(float) / Layout.GetStride());
		IndiceCount = uint32_t(Indices.size());
		
		auto VectexBufferDesc = FBufferDesc::VertexBufferDesc(VertexCount, Layout.GetStride());
		auto IndexBufferDesc = FBufferDesc::IndexBufferDesc(IndiceCount);
		m_IndexBuffer = CreateRef<FDX12Buffer>(IndexBufferDesc, Indices.data());
		m_VertexBuffer = CreateRef<FDX12Buffer>(VectexBufferDesc, Vertices.data());
	}

	void FDX12Mesh::Draw(FCommandListHandle CommandListHandle)
	{
		CORE_ASSERT(false, "false");
		Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		auto D3DCommandList = CommandList->GetD3D12CommandList();
		//D3DCommandList->IASetVertexBuffers(0, 1, &(m_D3DVertexBuffer->GetVertexBufferView()));
		//D3DCommandList->IASetIndexBuffer(&(m_D3DIndexBuffer->GetIndexBufferView()));
		D3DCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//D3DCommandList->DrawIndexedInstanced(m_D3DIndexBuffer->GetIndexCount(), 1, 0, 0, 0);
	}

	void FDX12Mesh::DrawSubMesh(FSubMesh& SubMesh, FCommandListHandle CommandListHandle)
	{
		Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		auto D3DCommandList = CommandList->GetD3D12CommandList();
		FDX12Device::Get()->BindVertexBuffer(CommandListHandle, m_VertexBuffer.get());
		FDX12Device::Get()->BindIndexBuffer(CommandListHandle, m_IndexBuffer.get());
		D3DCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3DCommandList->DrawIndexedInstanced(SubMesh.IndexNumber, 1, SubMesh.IndexLocation, SubMesh.VertexLocation, 0);
		//D3DCommandList->DrawInstanced(m_D3DVertexBuffer->m_VertexCount, 1, 0, 0);
	}
}