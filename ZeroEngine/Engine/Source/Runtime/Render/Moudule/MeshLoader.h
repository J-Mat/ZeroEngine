#pragma once

#include "Core.h"

namespace Zero
{
	struct FMeshData
	{
		std::vector<float> m_Vertices;
		std::vector<uint32_t> m_Indices;

		uint32_t m_VertexNum;
		uint32_t m_IndexNum;

		FMeshData() = default;
		FMeshData(const std::vector<float>& Vertices,
			const std::vector<uint32_t>& Indices,
			const uint32_t& VNum, const uint32_t& INum)
			:m_Vertices(Vertices), m_Indices(Indices), m_VertexNum(VNum), m_IndexNum(INum) {}
	};
}