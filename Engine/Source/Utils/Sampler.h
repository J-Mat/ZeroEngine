#pragma once
namespace Zero
{
	#define TAA_SAMPLE_COUNT 8
	namespace SamplerUtils
	{
		static const float Halton_2[8] =
		{
			0.0,
			-1.0 / 2.0,
			1.0 / 2.0,
			-3.0 / 4.0,
			1.0 / 4.0,
			-1.0 / 4.0,
			3.0 / 4.0,
			-7.0 / 8.0
		};

		// 8x TAA, (-1, 1)
		static const float Halton_3[8] =
		{
			-1.0 / 3.0,
			1.0 / 3.0,
			-7.0 / 9.0,
			-1.0 / 9.0,
			5.0 / 9.0,
			-5.0 / 9.0,
			1.0 / 9.0,
			7.0 / 9.0
		};
	}
}