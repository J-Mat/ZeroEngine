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

	struct FMeshType
	{
		EMeshShapeType MeshShapeType;
		std::string  Path;
	};

	class FMeshData;
	class FMeshCreator : public IPublicSingleton<FMeshCreator>
	{
	public:
		void CreatMesh(const FMeshType& MeshType, FMeshData& MeshData);
	private: 
		void CreateCube(FMeshData& Meshata);
	};

	struct FMeshData
	{
		std::vector<float> m_Vertices;
		std::vector<uint32_t> m_Indices;

		uint32_t m_VertexNum;
		uint32_t m_IndexNum;

		FMeshData() = default;
		FMeshData(const std::vector<float>& Vertices,
			const std::vector<uint32_t>& Indices)
			:m_Vertices(Vertices), m_Indices(Indices), m_VertexNum(m_Vertices.size()), m_IndexNum(m_Indices.size()) {}
	};
}