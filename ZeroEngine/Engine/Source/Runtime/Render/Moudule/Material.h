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
	class FShader;
	class IShaderBinder;
	struct FShaderConstantsDesc;
	struct FShaderResourcesDesc;
	class IDevice;
	class FTexture2D;
	class FTextureCubemap;
	class IShaderConstantsBuffer;
	class IShaderResourcesBuffer;
	class FMaterial
	{
	public:
		FMaterial();
		~FMaterial();	
		void Tick();
		void SetPass();
		void OnDrawCall();
		void SetShader(Ref<FShader> Shader);
		void SetShader(const std::string& ShaderFile);
		Ref<FShader> GetShader() { return m_Shader; }

		void SetFloat(const std::string& Name, const float& Value);
		void SetFloat3(const std::string& Name, const ZMath::vec3& Value);
		void SetFloat4(const std::string& Name, const ZMath::vec4& Value);
		void SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value);
		void SetTexture2D(const std::string& Name, Ref<FTexture2D> Texture);
		void SetTextureCubemap(const std::string& Name, Ref<FTextureCubemap> Texture);

		void GetFloat(const std::string& Name, float& Value);
		void GetFloat3(const std::string& Name, ZMath::vec3& Value);
		void GetFloat4(const std::string& Name, ZMath::vec4& Value);
		void GetMatrix4x4(const std::string& Name, ZMath::mat4& Value);

		float* PtrFloat(const std::string& Name);
		float* PtrFloat3(const std::string& Name);
		float* PtrFloat4(const std::string& Name);
		float* PtrMatrix4x4(const std::string& Name);
	private:
		Ref<FShader> m_Shader;
		Ref<IShaderBinder> m_ShaderBinder;
		Ref<FShaderConstantsDesc> m_MaterialDesc;
		Ref<FShaderResourcesDesc> m_ResourcesDesc;

		Ref<IShaderConstantsBuffer> m_MaterialBuffer;
		Ref<IShaderResourcesBuffer> m_ResourcesBuffer;
	};
}