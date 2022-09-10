#include "Core.h"
#include "MeshLoader.h"
#include  "Platform/DX12/Common/DX12Header.h"

namespace Zero
{
	void FMeshCreator::CreatMesh(const FMeshType& MeshType, FMeshData& MeshData)
	{
		switch (MeshType.MeshShapeType)
		{
		case EMeshShapeType::Custom:
			break;
		case EMeshShapeType::Cube:
			CreateCube(MeshData);
			break;
		default:
			break;
		}
	}
	void FMeshCreator::CreateCube(FMeshData& Meshata)
	{
		Meshata.m_Vertices =
		{
			-1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,
			-1.0f, +1.0f, -1.0f,   Utils::Colors::Black[0], Utils::Colors::Black[1], Utils::Colors::Black[2],
			+1.0f, +1.0f, -1.0f,   Utils::Colors::Red[0],  Utils::Colors::Red[1], Utils::Colors::Red[2],
			+1.0f, -1.0f, -1.0f,   Utils::Colors::Green[0], Utils::Colors::Green[1], Utils::Colors::Green[2],
			-1.0f, -1.0f, +1.0f,   Utils::Colors::Blue[0], Utils::Colors::Blue[1], Utils::Colors::Blue[2],
			-1.0f, +1.0f, +1.0f,   Utils::Colors::Yellow[0], Utils::Colors::Yellow[1], Utils::Colors::Yellow[2]
			+ 1.0f, +1.0f, +1.0f,  Utils::Colors::Cyan[0], Utils::Colors::Cyan[1], Utils::Colors::Cyan[2],
			+1.0f, -1.0f, +1.0f,   Utils::Colors::Magenta[0], Utils::Colors::Magenta[1], Utils::Colors::Magenta[2]
		};
		
		Meshata.m_VertexNum = Meshata.m_Vertices.size();

		std::vector<uint32_t> Indices =
		{
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};
		
		Meshata.m_IndexNum = Meshata.m_Indices.size();
	}
}
