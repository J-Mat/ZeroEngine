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
		FMeshType(EMeshShapeType Type, std::string _Path) 
			: MeshShapeType(Type)
			, Path(_Path)
		{}
		EMeshShapeType MeshShapeType = EMeshShapeType::Custom;
		std::string  Path = "";
		static FMeshType s_CubeMeshType; 
	};

	struct FMeshData;
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

		FMeshData() = default;
		FMeshData(const std::vector<float>& Vertices, const std::vector<uint32_t>& Indices)
			:m_Vertices(Vertices), 
			m_Indices(Indices)
		{}
	};
}