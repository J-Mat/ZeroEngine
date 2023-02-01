#pragma once

#include "Core.h"
#include "Core/Base/PublicSingleton.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Render/RHI/VertexBuffer.h"

namespace Zero
{
	enum  EMeshShapeType
	{
		Custom,
		Cube,
		Sphere,
	};

	struct FVertex
	{
		FVertex() = default;
		FVertex(
			const ZMath::vec3& _Pos,
			const ZMath::vec3& _N,
			const ZMath::vec3& _Tangent,
			const ZMath::vec2& UV) :
			Position(_Pos),
			Normal(_N),
			TangentU(_Tangent),
			TexC(UV) {}
		FVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		ZMath::vec3 Position;
		ZMath::vec3 Normal;
		ZMath::vec3 TangentU;
		ZMath::vec2 TexC;
	};

	struct FMeshType
	{
		FMeshType() {};
		FMeshType(EMeshShapeType Type, std::string _Path) 
			: MeshShapeType(Type)
			, Path(_Path)
		{}
		EMeshShapeType MeshShapeType = EMeshShapeType::Custom;
		std::string  Path = "";
		static FMeshType s_CubeMeshType; 	
		static FMeshType s_SphereMeshType; 	
	};

	struct FMeshData
	{
		ZMath::FAABB AABB;	
		std::vector<float> Vertices;
		std::vector<uint32_t> Indices;

		FMeshData() = default;
		FMeshData(const std::vector<float>& _Vertices, const std::vector<uint32_t>& _Indices)
			:Vertices(_Vertices),
			Indices(_Indices)
		{}
	};

	class FMeshGenerator : public IPublicSingleton<FMeshGenerator>
	{
	public:
		void CreateCube(FMeshData& Meshata, float Width, float Height, float Depth, uint32_t NumSubdivisions);
		void CreateSphere(FMeshData& MeshData, float Radius, uint32_t NumSubdivisions);
		void CreateGrid(FMeshData& MeshData, float width, float Depth, uint32_t m, uint32_t n);
		void CreateRect(FMeshData& MeshData);
		void CreateCustomModel(std::vector<FMeshData>& MeshDatas, const std::string& Path, FVertexBufferLayout& Layout);
	private: 

		void ProcessNode(std::vector<FMeshData>& MeshDatas, aiNode* Node, const aiScene* Scene, FVertexBufferLayout& Layout);
		FMeshData ProcessMesh(aiMesh* Mesh, const aiScene* Scene, FVertexBufferLayout& Layout);
		FVertex MidPoint(const FVertex& V0, const FVertex& V1);
		void SubDivide(std::vector<FVertex>& Vertexes, std::vector<uint32_t>& Indices);
		void AttachToMeshData(FMeshData& MeshData, std::vector<FVertex>& Vertexes, std::vector<uint32_t>& Indices, FVertexBufferLayout& VertexLayout = FVertexBufferLayout::s_DefaultVertexLayout);
	};



}