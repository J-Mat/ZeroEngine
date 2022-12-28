#include "Core.h"
#include "MeshGenerator.h"
#include "Platform/DX12/Common/DX12Header.h"
#include <stdarg.h>
#include "Render/RHI/VertexBuffer.h"

namespace Zero
{
	FMeshType FMeshType::s_CubeMeshType = { EMeshShapeType::Cube, "" };
	FMeshType FMeshType::s_SphereMeshType = { EMeshShapeType::Sphere, "" };

	void FMeshGenerator::CreateCube(FMeshData& MeshData, float Width, float Height, float Depth, uint32_t NumSubdivisions)
	{
		std::vector<FVertex> Vertex(24);
		
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
		

		std::vector<uint32_t> Indexs(36);

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


		NumSubdivisions = std::min(NumSubdivisions, 6u);
		
		while (NumSubdivisions--)
		{
			SubDivide(Vertex, Indexs);
		}

		AttachToMeshData(MeshData, Vertex, Indexs);
	}

	void FMeshGenerator::CreateRect(FMeshData& MeshData)
	{
		std::vector<FVertex> Vertex(4);
		
		float W2 = 0.5f;
		float H2 = 0.5f;
		// Fill in the front face vertex data.
		Vertex[0] = FVertex(-W2, -H2, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		Vertex[1] = FVertex(-W2, +H2, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex[2] = FVertex(+W2, +H2, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		Vertex[3] = FVertex(+W2, -H2, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		std::vector<uint32_t> Indexs(6);

		// Fill in the front face index data
		Indexs[0] = 0; Indexs[1] = 1; Indexs[2] = 2;
		Indexs[3] = 0; Indexs[4] = 2; Indexs[5] = 3;

		AttachToMeshData(MeshData, Vertex, Indexs);
	}

	void FMeshGenerator::CreateSphere(FMeshData& MeshData, float Radius, uint32_t NumSubdivisions)
	{
		// Put a cap on the number of subdivisions.
		NumSubdivisions = std::min(NumSubdivisions, 6u);

		// Approximate a sphere by tessellating an icosahedron.

		const float X = 0.525731f;
		const float Z = 0.850651f;

		ZMath::vec3 pos[12] =
		{
			ZMath::vec3 (-X, 0.0f, Z),  ZMath::vec3 (X, 0.0f, Z),
			ZMath::vec3 (-X, 0.0f, -Z), ZMath::vec3 (X, 0.0f, -Z),
			ZMath::vec3 (0.0f, Z, X),   ZMath::vec3 (0.0f, Z, -X),
			ZMath::vec3 (0.0f, -Z, X),  ZMath::vec3 (0.0f, -Z, -X),
			ZMath::vec3 (Z, X, 0.0f),   ZMath::vec3 (-Z, X, 0.0f),
			ZMath::vec3 (Z, -X, 0.0f),  ZMath::vec3 (-Z, -X, 0.0f)
		};

		std::vector<uint32_t> Indexs = 
		{
			1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
			1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		};

		std::vector<FVertex> Vertices(12);

		for (uint32_t i = 0; i < 12; ++i)
			Vertices[i].Position = pos[i];

		while (NumSubdivisions--)
		{
			SubDivide(Vertices, Indexs);
		}

		// Project vertices onto sphere and scale.
		for (uint32_t i = 0; i < Vertices.size(); ++i)
		{
			// Project onto unit sphere.
			ZMath::vec3 n = ZMath::normalize(Vertices[i].Position);

			// Project onto sphere.
			ZMath::vec3 p = Radius * n;

			Vertices[i].Position = p;
			Vertices[i].Normal =  n;

			// Derive texture coordinates from spherical coordinates.
			float theta = atan2f(Vertices[i].Position.z,Vertices[i].Position.x);

			// Put in [0, 2pi].
			if (theta < 0.0f)
			{
				theta += ZMath::pi<float>() * 2;
			}

			float phi = acosf(Vertices[i].Position.y / Radius);

			Vertices[i].TexC.x = theta / (ZMath::pi<float>() * 2);
			Vertices[i].TexC.y = phi / ZMath::pi<float>();

			// Partial derivative of P with respect to theta
			Vertices[i].TangentU.x = -Radius * sinf(phi) * sinf(theta);
			Vertices[i].TangentU.y = 0.0f;
			Vertices[i].TangentU.z = +Radius * sinf(phi) * cosf(theta);
		}

		AttachToMeshData(MeshData, Vertices, Indexs);
	}

	void FMeshGenerator::CreateCustomModel(std::vector<FMeshData>& MeshDatas, const std::string& Path, FVertexBufferLayout& Layout)
	{
		Assimp::Importer Importer;
		unsigned int Flag = aiProcess_Triangulate;
		for (const FBufferElement& Element : Layout)
		{
			if (Element.Name == "NORMAL")
				Flag |= aiProcess_GenNormals;
			if (Element.Name == "TANGENT")
				Flag |= aiProcess_CalcTangentSpace;
		}
		const aiScene* Scene = Importer.ReadFile(Path, Flag);

		if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
		{
			CORE_LOG_ERROR("Mesh Load Error");
			return;
		}
		ProcessNode(MeshDatas, Scene->mRootNode, Scene, Layout);
	}

	void FMeshGenerator::ProcessNode(std::vector<FMeshData>& MeshDatas, aiNode* Node, const aiScene* Scene, FVertexBufferLayout& Layout)
	{
		for (unsigned int i = 0; i < Node->mNumMeshes; i++)
		{
			aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
			MeshDatas.push_back(ProcessMesh(Mesh, Scene, Layout));
		}
		// Iteratorly Process Child Node
		for (unsigned int i = 0; i < Node->mNumChildren; i++)
		{
			ProcessNode(MeshDatas, Node->mChildren[i], Scene, Layout);
		}
	}

	FMeshData FMeshGenerator::ProcessMesh(aiMesh* Mesh, const aiScene* Scene, FVertexBufferLayout& Layout)
	{
		FMeshData MeshData;

		for (unsigned int i = 0; i < Mesh->mNumVertices; i++)
		{
			for (FBufferElement& Element : Layout)
			{
				if (Element.Name == "POSITION")
				{
					MeshData.Vertices.push_back(Mesh->mVertices[i].x);
					MeshData.Vertices.push_back(Mesh->mVertices[i].y);
					MeshData.Vertices.push_back(Mesh->mVertices[i].z);
					MeshData.AABB.Merge({ Mesh->mVertices[i].x, Mesh->mVertices[i].y ,Mesh->mVertices[i].z });
				}
				else if (Element.Name == "TEXCOORD" || Element.Name == "TEXCO0RD0")
				{
					if (Mesh->HasTextureCoords(0))
					{
						MeshData.Vertices.push_back(Mesh->mTextureCoords[0][i].x);
						MeshData.Vertices.push_back(Mesh->mTextureCoords[0][i].y);
					}
					else
					{
						MeshData.Vertices.push_back(0);
						MeshData.Vertices.push_back(0);
					}
				}
				else if (Element.Name == "NORMAL")
				{
					MeshData.Vertices.push_back(Mesh->mNormals[i].x);
					MeshData.Vertices.push_back(Mesh->mNormals[i].y);
					MeshData.Vertices.push_back(Mesh->mNormals[i].z);
				}
				else if (Element.Name == "TANGENT")
				{
					if (Mesh->HasTangentsAndBitangents())
					{
						MeshData.Vertices.push_back(Mesh->mTangents[i].x);
						MeshData.Vertices.push_back(Mesh->mTangents[i].y);
						MeshData.Vertices.push_back(Mesh->mTangents[i].z);
					}
					else
					{
						//SIByL_CORE_ERROR("Mesh Process Error: NO TANGENT");
						MeshData.Vertices.push_back(0);
						MeshData.Vertices.push_back(0);
						MeshData.Vertices.push_back(0);
					}
				}
				else
				{
					for (uint32_t j = 0; j < Element.Size / 4; j++)
					{
						MeshData.Vertices.push_back(0);
					}
				}
			}
		}

		for (uint32_t i = 0; i < Mesh->mNumFaces; i++)
		{
			aiFace face = Mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				MeshData.Indices.push_back(face.mIndices[j]);
		}

		return MeshData;
	}

	FVertex FMeshGenerator::MidPoint(const FVertex& v0, const FVertex& v1)
	{
		ZMath::vec3 p0 = v0.Position;
		ZMath::vec3 p1 = v1.Position;

		ZMath::vec3 n0 = v0.Normal;
		ZMath::vec3 n1 = v1.Normal;

		ZMath::vec3 tan0 = v0.TangentU;
		ZMath::vec3 tan1 = v1.TangentU;

		ZMath::vec2 tex0 = v0.TexC;
		ZMath::vec2 tex1 = v1.TexC;

		ZMath::vec3 pos = 0.5f * (p0 + p1);
		ZMath::vec3 normal = ZMath::normalize(n0 + n1);
		ZMath::vec3 tangent = ZMath::normalize(tan0 + tan1);
		ZMath::vec2 tex = 0.5f * (tex0 + tex1);

		FVertex v;
		v.Position = pos;
		v.Normal = normal;
		v.TangentU = tangent;
		v.TexC = tex;

		return v;
	}

	void FMeshGenerator::SubDivide(std::vector<FVertex>& Vertices, std::vector<uint32_t>& Indices)
	{
		std::vector<FVertex> InputVertex = Vertices;
		std::vector<uint32_t> InputIndice = Indices;

		Vertices.resize(0);
		Indices.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		uint32_t NumTris = (uint32_t)InputIndice.size() / 3;
		for (uint32_t i = 0; i < NumTris; ++i)
		{
			FVertex v0 = InputVertex[InputIndice[i * 3 + 0]];
			FVertex v1 = InputVertex[InputIndice[i * 3 + 1]];
			FVertex v2 = InputVertex[InputIndice[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			FVertex m0 = MidPoint(v0, v1);
			FVertex m1 = MidPoint(v1, v2);
			FVertex m2 = MidPoint(v0, v2);

			//
			// Add new geometry.
			//

			Vertices.push_back(v0); // 0
			Vertices.push_back(v1); // 1
			Vertices.push_back(v2); // 2
			Vertices.push_back(m0); // 3
			Vertices.push_back(m1); // 4
			Vertices.push_back(m2); // 5

			Indices.push_back(i * 6 + 0);
			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 5);

			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 4);
			Indices.push_back(i * 6 + 5);

			Indices.push_back(i * 6 + 5);
			Indices.push_back(i * 6 + 4);
			Indices.push_back(i * 6 + 2);

			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 1);
			Indices.push_back(i * 6 + 4);
		}
	}
	void FMeshGenerator::AttachToMeshData(FMeshData& MeshData, std::vector<FVertex>& Vertexes, std::vector<uint32_t>& Indices)
	{
		for(FVertex& Vertex : Vertexes)
		{
			MeshData.Vertices.push_back(Vertex.Position.x);
			MeshData.Vertices.push_back(Vertex.Position.y);
			MeshData.Vertices.push_back(Vertex.Position.z);
			MeshData.AABB.Merge(Vertex.Position);
			
			MeshData.Vertices.push_back(Vertex.Normal.x);
			MeshData.Vertices.push_back(Vertex.Normal.y);
			MeshData.Vertices.push_back(Vertex.Normal.z);

			MeshData.Vertices.push_back(Vertex.TangentU.x);
			MeshData.Vertices.push_back(Vertex.TangentU.y);
			MeshData.Vertices.push_back(Vertex.TangentU.z);
			
			MeshData.Vertices.push_back(Vertex.TexC.x);
			MeshData.Vertices.push_back(Vertex.TexC.y);
		}
		
		MeshData.Indices.assign(Indices.begin(), Indices.end());
	}
}
