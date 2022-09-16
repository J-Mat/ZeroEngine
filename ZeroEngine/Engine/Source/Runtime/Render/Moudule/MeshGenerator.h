#pragma once

#include "Core.h"
#include "Core/Base/PublicSingleton.h"

namespace Zero
{
	enum  EMeshShapeType
	{
		Custom,
		Cube
	};

	struct FVertex
	{
		FVertex() {}
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
		FMeshType(EMeshShapeType Type, std::string _Path) 
			: MeshShapeType(Type)
			, Path(_Path)
		{}
		EMeshShapeType MeshShapeType = EMeshShapeType::Custom;
		std::string  Path = "";
		static FMeshType s_CubeMeshType; 	
	};

	struct FMeshData
	{
		std::vector<float> Vertices;
		std::vector<uint32_t> Indices;

		FMeshData() = default;
		FMeshData(const std::vector<float>& _Vertices, const std::vector<uint32_t>& _Indices)
			:Vertices(_Vertices),
			Indices(_Indices)
		{}
	};

	class FMeshCreator : public IPublicSingleton<FMeshCreator>
	{
	public:
		void CreatMesh(const FMeshType& MeshType, FMeshData& MeshData, int ParaNum, ...);
	private: 
		void CreateCube(FMeshData& Meshata, float Width, float Height, float Depth, uint32_t NumSubdivisions);

		void SubDivide(FMeshData& MeshData);
	};



}