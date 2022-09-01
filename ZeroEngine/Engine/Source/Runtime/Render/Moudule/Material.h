#pragma once
#include "Core.h"


namespace Zero
{
	enum class EAlphaState
	{
		Opaque,
		AlphaTest,
		AlphaBlend,
	};

	struct FPipelineStateDesc
	{
		EAlphaState AlphaState;
	};
	class IShader;
	class FMaterial
	{
	public:
		FMaterial();
		~FMaterial();	
		void SetShader(Ref<IShader> Shader);
	private:
		Ref<IShader> m_Shader;
	};
}