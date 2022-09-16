#include "Core.h"
#include "MeshGenerator.h"
#include  "Platform/DX12/Common/DX12Header.h"
#include <stdarg.h>

namespace Zero
{
	FMeshType FMeshType::s_CubeMeshType = { EMeshShapeType::Cube, "" };

	void FMeshCreator::CreatMesh(const FMeshType& MeshType, FMeshData& MeshData, int ParaNum, ...)
	{
		switch (MeshType.MeshShapeType)
		{
		case EMeshShapeType::Custom:
			break;
		case EMeshShapeType::Cube:
		{
			va_list VaList;
			va_start(VaList, ParaNum);
			float Width = va_arg(VaList, float);
			float Height = va_arg(VaList, float);
			float Depth = va_arg(VaList, float);
			uint32_t NumSubdivisions = va_arg(VaList, uint32_t);
			va_end(VaList);
			CreateCube(MeshData, Width, Height, );
			break;
		}
		default:
			break;
		}
	}
	void FMeshCreator::CreateCube(FMeshData& MeshData, float Width, float Height, float Depth, uint32_t NumSubdivisions)
	{
		FVertex Vertex[24];
		
		float W2 = 0.5f * Width;
		float H2 = 0.5f * Height;
		float D2 = 0.5f * Depth; 
		// Fill in the front face vertex data.
		Vertex[0] = FVertex(-W2, -H2, -D2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		Vertex[1] = FVertex(-W2, +H2, -D2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex[2] = FVertex(+W2, +H2, -D2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		Vertex[3] = FVertex(+W2, -H2, -D2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		Vertex[4] = FVertex(-W2, -H2, +D2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[5] = FVertex(+W2, -H2, +D2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		Vertex[6] = FVertex(+W2, +H2, +D2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex[7] = FVertex(-W2, +H2, +D2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		Vertex[8] = FVertex(-W2, +H2, -D2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		Vertex[9] = FVertex(-W2, +H2, +D2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex[10] = FVertex(+W2, +H2, +D2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		Vertex[11] = FVertex(+W2, +H2, -D2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		Vertex[12] = FVertex(-W2, -H2, -D2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[13] = FVertex(+W2, -H2, -D2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		Vertex[14] = FVertex(+W2, -H2, +D2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex[15] = FVertex(-W2, -H2, +D2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		Vertex[16] = FVertex(-W2, -H2, +D2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		Vertex[17] = FVertex(-W2, +H2, +D2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		Vertex[18] = FVertex(-W2, +H2, -D2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		Vertex[19] = FVertex(-W2, -H2, -D2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		Vertex[20] = FVertex(+W2, -H2, -D2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[21] = FVertex(+W2, +H2, -D2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		Vertex[22] = FVertex(+W2, +H2, +D2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		Vertex[23] = FVertex(+W2, -H2, +D2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		
		MeshData.Vertices.assign(&Vertex[0], &Vertex[24]);

		uint32_t Indexs[36];

		// Fill in the front face index data
		Indexs[0] = 0; Indexs[1] = 1; Indexs[2] = 2;
		Indexs[3] = 0; Indexs[4] = 2; Indexs[5] = 3;

		// Fill in the back face index data
		Indexs[6] = 4; Indexs[7] = 5; Indexs[8] = 6;
		Indexs[9] = 4; Indexs[10] = 6; Indexs[11] = 7;

		// Fill in the top face index data
		Indexs[12] = 8; Indexs[13] = 9; Indexs[14] = 10;
		Indexs[15] = 8; Indexs[16] = 10; Indexs[17] = 11;

		// Fill in the bottom face index data
		Indexs[18] = 12; Indexs[19] = 13; Indexs[20] = 14;
		Indexs[21] = 12; Indexs[22] = 14; Indexs[23] = 15;

		// Fill in the left face index data
		Indexs[24] = 16; Indexs[25] = 17; Indexs[26] = 18;
		Indexs[27] = 16; Indexs[28] = 18; Indexs[29] = 19;

		// Fill in the right face index data
		Indexs[30] = 20; Indexs[31] = 21; Indexs[32] = 22;
		Indexs[33] = 20; Indexs[34] = 22; Indexs[35] = 23;

		MeshData.Indices.assign(&Indexs[0], &Indexs[24]);

		NumSubdivisions = std::min(NumSubdivisions, 6u);
		
		while (NumSubdivisions--)
		{
			SubDivide(MeshData);
		}
	}
	void FMeshCreator::SubDivide(FMeshData& MeshData)
	{
		FMeshData InputCopy = MeshData;

		MeshData.Vertices.resize(0);
		MeshData.Indices.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		uint32_t numTris = (uint32_t)InputCopy.Indices.size() / 3;
		for (uint32_t i = 0; i < numTris; ++i)
		{
			FVertex v0 = InputCopy.Vertices[InputCopy.Indices[i * 3 + 0]];
			FVertex v1 = InputCopy.Vertices[InputCopy.Indices[i * 3 + 1]];
			FVertex v2 = InputCopy.Vertices[InputCopy.Indices[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			FVertex m0 = MidPoint(v0, v1);
			FVertex m1 = MidPoint(v1, v2);
			FVertex m2 = MidPoint(v0, v2);

			//
			// Add new geometry.
			//

			MeshData.Vertices.push_back(v0); // 0
			MeshData.Vertices.push_back(v1); // 1
			MeshData.Vertices.push_back(v2); // 2
			MeshData.Vertices.push_back(m0); // 3
			MeshData.Vertices.push_back(m1); // 4
			MeshData.Vertices.push_back(m2); // 5

			MeshData.Indices.push_back(i * 6 + 0);
			MeshData.Indices.push_back(i * 6 + 3);
			MeshData.Indices.push_back(i * 6 + 5);

			MeshData.Indices.push_back(i * 6 + 3);
			MeshData.Indices.push_back(i * 6 + 4);
			MeshData.Indices.push_back(i * 6 + 5);

			MeshData.Indices.push_back(i * 6 + 5);
			MeshData.Indices.push_back(i * 6 + 4);
			MeshData.Indices.push_back(i * 6 + 2);

			MeshData.Indices.push_back(i * 6 + 3);
			MeshData.Indices.push_back(i * 6 + 1);
			MeshData.Indices.push_back(i * 6 + 4);
		}
	}
}
