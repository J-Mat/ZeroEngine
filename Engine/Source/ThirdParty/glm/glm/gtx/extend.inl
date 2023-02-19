/// @ref gtx_extend

namespace glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER genType extend
	(
		genType const& m_Origin,
		genType const& Source,
		genType const& Distance
	)
	{
		return m_Origin + (Source - m_Origin) * Distance;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<2, T, Q> extend
	(
		vec<2, T, Q> const& m_Origin,
		vec<2, T, Q> const& Source,
		T const& Distance
	)
	{
		return m_Origin + (Source - m_Origin) * Distance;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> extend
	(
		vec<3, T, Q> const& m_Origin,
		vec<3, T, Q> const& Source,
		T const& Distance
	)
	{
		return m_Origin + (Source - m_Origin) * Distance;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<4, T, Q> extend
	(
		vec<4, T, Q> const& m_Origin,
		vec<4, T, Q> const& Source,
		T const& Distance
	)
	{
		return m_Origin + (Source - m_Origin) * Distance;
	}
}//namespace glm
