#include "Core.h"
#include "MeshLoader.h"
#include  "Platform/DX12/Common/DX12Header.h"

namespace Zero
{
	FMeshType FMeshType::s_CubeMeshType = { EMeshShapeType::Cube, "" };

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
			-1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f, 1.0f,
			-1.0f, +1.0f, -1.0f,   Utils::Colors::Black[0], Utils::Colors::Black[1], Utils::Colors::Black[2], Utils::Colors::Black[3],
			+1.0f, +1.0f, -1.0f,   Utils::Colors::Red[0],  Utils::Colors::Red[1], Utils::Colors::Red[2],Utils::Colors::Red[3],
			+1.0f, -1.0f, -1.0f,   Utils::Colors::Green[0], Utils::Colors::Green[1], Utils::Colors::Green[2],Utils::Colors::Green[3],
			-1.0f, -1.0f, +1.0f,   Utils::Colors::Blue[0], Utils::Colors::Blue[1], Utils::Colors::Green[2],Utils::Colors::Green[3],
			-1.0f, +1.0f, +1.0f,   Utils::Colors::Yellow[0], Utils::Colors::Yellow[1], Utils::Colors::Yellow[2], Utils::Colors::Yellow[3],
			+1.0f, +1.0f, +1.0f,   Utils::Colors::Cyan[0], Utils::Colors::Cyan[1], Utils::Colors::Cyan[2], Utils::Colors::Cyan[3],
			+1.0f, -1.0f, +1.0f,   Utils::Colors::Magenta[0], Utils::Colors::Magenta[1], Utils::Colors::Magenta[2], Utils::Colors::Magenta[3],
		};
		

		Meshata.m_Indices =
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
		
	}
}
