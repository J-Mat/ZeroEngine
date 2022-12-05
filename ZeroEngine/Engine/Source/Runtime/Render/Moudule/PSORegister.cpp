#include "PSORegister.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RenderConfig.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/PipelineStateObject.h"

namespace Zero
{
	void FPSORegister::RegisterDefaultPSO()
	{
		std::vector<FConstantBufferLayout> CBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			FConstantBufferLayout::s_PerCameraConstants,
			// Material
			{
				{EShaderDataType::RGBA, "Base"},
			},
			FConstantBufferLayout::s_PerFrameConstants
		};

		FShaderResourceLayout ResourceLayout =
		{
			{EShaderResourceType::Texture2D, "gDiffuseMap"},
			{EShaderResourceType::Cubemap, "gSkyboxMap"}
		};

		FFrameBufferTexturesFormats Formats = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::None, //   For Picking
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::DEPTH32F
		};

		FShaderDesc ShaderDesc = { false, FVertexBufferLayout::s_DefaultVertexLayout, 2, Formats };

		FPSODescriptor ForwadLitDesc;
		FShaderBinderDesc  ShaderBinderDesc = { CBLayouts, ResourceLayout };
		ShaderBinderDesc.m_PerObjIndex = 0;
		ShaderBinderDesc.m_CameraIndex = 1;
		ShaderBinderDesc.m_MaterialIndex = 2;
		ShaderBinderDesc.m_ConstantIndex = 3;
		ForwadLitDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/ForwardLit.hlsl", ShaderBinderDesc, ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_FORWARDLIT, ForwadLitDesc);

		FPSODescriptor LightDesc;
		LightDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/DirectLight.hlsl", ShaderBinderDesc, ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_LIGHT, LightDesc);


		Ref<FShader> Shader = FRenderer::GraphicFactroy->CreateShader("Shader/Test.hlsl", ShaderDesc);

	}

	void FPSORegister::RegisterSamplePSO()
	{
		std::vector<FConstantBufferLayout> FresnelCBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			FConstantBufferLayout::s_PerCameraConstants,
			// Material
			{
				{EShaderDataType::Float, "Fresnel_1"},
				{EShaderDataType::Float, "Fresnel_2"},
				{EShaderDataType::Float, "Fresnel_3"},
			},
			FConstantBufferLayout::s_PerFrameConstants
		};


		FFrameBufferTexturesFormats Formats = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::None, 
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::DEPTH32F
		};


		FShaderDesc ShaderDesc = { false, FVertexBufferLayout::s_DefaultVertexLayout, 2, Formats };

		FPSODescriptor FresnelDesc;
		FShaderBinderDesc  FresnelShaderBinderDesc = { FresnelCBLayouts};
		FresnelShaderBinderDesc.m_PerObjIndex = 0;
		FresnelShaderBinderDesc.m_CameraIndex = 1;
		FresnelShaderBinderDesc.m_MaterialIndex = 2;
		FresnelShaderBinderDesc.m_ConstantIndex = 3;
		FresnelDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/Fresnel.hlsl", FresnelShaderBinderDesc, ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_FRESNEL, FresnelDesc);

		std::vector<FConstantBufferLayout> NDFCBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			FConstantBufferLayout::s_PerCameraConstants,
			// Material
			{
				{EShaderDataType::Float, "Rougness"},
			},
			FConstantBufferLayout::s_PerFrameConstants
		};



		FPSODescriptor NDFDesc;
		FShaderBinderDesc  NDFShaderBinderDesc = { NDFCBLayouts };
		NDFShaderBinderDesc.m_PerObjIndex = 0;
		NDFShaderBinderDesc.m_CameraIndex = 1;
		NDFShaderBinderDesc.m_MaterialIndex = 2;
		NDFShaderBinderDesc.m_ConstantIndex = 3;
		NDFDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/NDF.hlsl", NDFShaderBinderDesc, ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_NDF, NDFDesc);

		std::vector<FConstantBufferLayout> GGXCBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			FConstantBufferLayout::s_PerCameraConstants,
			// Material
			{
				{EShaderDataType::Float, "Rougness"},
			},
			FConstantBufferLayout::s_PerFrameConstants
		};


		/*
		FPSODescriptor GGXDesc;
		FShaderBinderDesc  GGXShaderBinderDesc = { GGXCBLayouts };
		GGXShaderBinderDesc.m_PerObjIndex = 0;
		GGXShaderBinderDesc.m_CameraIndex = 1;
		GGXShaderBinderDesc.m_MaterialIndex = 2;
		GGXShaderBinderDesc.m_ConstantIndex = 3;
		GGXDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/GGX.hlsl", GGXShaderBinderDesc, ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_GGX, GGXDesc);
		*/
	}

	void FPSORegister::RegisterSkyboxPSO()
	{
		std::vector<FConstantBufferLayout> CBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			FConstantBufferLayout::s_PerCameraConstants,
		};

		FShaderResourceLayout ResourceLayout =
		{
			{EShaderResourceType::Cubemap, "gSkyboxMap"}
		};

		FFrameBufferTexturesFormats Formats = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::DEPTH32F
		};

		FShaderDesc ShaderDessc = { false, FVertexBufferLayout::s_DefaultVertexLayout, 2, Formats };

		FShaderBinderDesc  ShaderBinderDesc = { CBLayouts, ResourceLayout };
		ShaderBinderDesc.m_PerObjIndex = 0;
		ShaderBinderDesc.m_CameraIndex = 1;

		FPSODescriptor SkyboxPSODesc;
		SkyboxPSODesc.CullMode = ECullMode::CULL_MODE_FRONT;
		SkyboxPSODesc.DepthFunc = EComparisonFunc::LESS_EQUAL;
		SkyboxPSODesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/Skybox.hlsl", ShaderBinderDesc, ShaderDessc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_SKYBOX, SkyboxPSODesc);
	}
}
