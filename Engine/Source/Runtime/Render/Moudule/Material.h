#pragma once
#include "Core.h"
#include "Delegate.h"
#include "Render/RHI/Shader/ShaderData.h"
#include "Render/RenderConfig.h"

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
	class FTextureCube;
	class IShaderConstantsBuffer;
	class IShaderResourcesBuffer;
	DEFINITION_SIMPLE_SINGLE_DELEGATE(FOnSetParameter, void, const std::string&, void*)
	class FMaterial
	{
	public:
		FMaterial(bool bUseMainCamera = true);
		~FMaterial();	
		void Tick();
		void SetPass(FCommandListHandle CommandListHanle);
		void OnDrawCall(FCommandListHandle CommandListHanle);
		void SetShader(Ref<FShader> Shader);
		Ref<FShader> GetShader() { return m_Shader; }

		void SetFloat(const std::string& Name, const float& Value);
		void SetFloatPtr(const std::string& Name, void* Value);
		void SetInt(const std::string& Name, const int32_t& Value);
		void SetIntPtr(const std::string& Name, void* ValueiPtr);
		void SetFloat2(const std::string& Name, const ZMath::vec2& Value);
		void SetFloat3(const std::string& Name, const ZMath::vec3& Value);
		void SetFloat4(const std::string& Name, const ZMath::vec4& Value);
		void SetFloat4Ptr(const std::string& Name, void* ValuePtr);
		void SetMatrix4x4(const std::string& Name, const ZMath::mat4& Value);
		void SetCameraProjectMat(const std::string& Name, const ZMath::mat4& Value);
		void SetCameraViewMat(const std::string& Name, const ZMath::mat4& Value);
		void SetCameraProjectViewMat(const std::string& Name, const ZMath::mat4& Value);
		void SetCameraViewPos(const std::string& Name, const ZMath::vec3& Value);
		void SetTexture2D(const std::string& Name, FTexture2D* Texture);
		void SetTexture2DArray(const std::string& Name, const std::vector<Ref<FTexture2D>> Textures);
		void SetTextureCubemap(const std::string& Name, Ref<FTextureCube> Texture);
		void SetTextureCubemapArray(const std::string& Name, const std::vector<Ref<FTextureCube>>& Textures);

		void SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr);

		void GetFloat(const std::string& Name, float& Value);
		void GetFloat3(const std::string& Name, ZMath::vec3& Value);
		void GetFloat4(const std::string& Name, ZMath::vec4& Value);
		void GetMatrix4x4(const std::string& Name, ZMath::mat4& Value);

		float* PtrFloat(const std::string& Name);
		float* PtrFloat3(const std::string& Name);
		float* PtrFloat4(const std::string& Name);
		float* PtrMatrix4x4(const std::string& Name);

		bool IsSetIBL() { return m_bSetIBL; }
		void SetIBL(bool bValue) { m_bSetIBL = bValue; };
	private:
		std::map<EShaderDataType, FOnSetParameter&> m_SetParameterMap;
		Ref<FShader> m_Shader;
		Ref<IShaderBinder> m_ShaderBinder;
		Ref<FShaderConstantsDesc> m_MaterialDesc;
		Ref<FShaderResourcesDesc> m_ResourcesDesc;

		Ref<IShaderConstantsBuffer> m_MaterialBuffer = nullptr;
		Ref<IShaderConstantsBuffer> m_CameraBuffer = nullptr;
		Ref<IShaderResourcesBuffer> m_ResourcesBuffer = nullptr;
		
		bool m_bUseMainCamera = true;
		bool m_bSetIBL = false;
	private:
		FOnSetParameter m_OnSetFloat;
		FOnSetParameter m_OnSetInt;
	};
}