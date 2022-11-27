# ZeroEngine

- [LearningDX12](https://github.com/J-Mat/ZeroEngine/tree/main/LearningDX12 "LearningDX12")  龙书源码，用cmake整合在一起， 方便自己看
- [ZeroEngine](https://github.com/J-Mat/ZeroEngine/tree/main/ZeroEngine "ZeroEngine") 自制玩具

## MileStone

### ClearValue **2022-08-23**

![1663596672383](image/README/1663596672383.png)

### Hello Triangle **2022-09-14**

![1663596830138](image/README/1663596830138.png)

### Hello Texture Box  **2022-09-19**

![1663596898141](image/README/1663596898141.png)

### Hello imgui  **2022-09-22**

![1663845620594](image/README/1663845620594.png)

### Editor framework based imgui & ImGuizmo &  Code Reflection **2022-10-23**

you can modify the objects' properties through DetailPanel  based code reflection (**reference to UE4**)

![1666530959987](image/README/1666530959987.png)

### Save scnen based dode reflection (using yaml) **2022-10-31**

![1667201681377](image/README/1667201681377.png)

### **DirectLight Control 2022-11-5**

![1667651908997](image/README/1667651908997.png)

### Simple Material System 2022-11-17

I converted the map and vector data to UI panel through code reflection.

You can DIY the slide floats, colos, and textures in the material file, attaching these parameters to the shader and preparing the comming PBR effect in the future!

 ![1668840995552](image/README/1668840995552.png)

### Skybox 2022-11-27

I've disjoined shader and PSO into 2 parts. So we can config PSO settings, such as cull mode, depth compair func, blend state, etc.

Absolutely the skybox need different PSO, DepthFunc is LESS_EQUAL, CullMode is CULL_MODE_FRONT.

```cpp
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

	FShaderDesc ShaderDessc = { false, FVertexBufferLayout::s_SkyboxVertexLayout, 2, Formats };

	FShaderBinderDesc  ShaderBinderDesc = { CBLayouts, ResourceLayout };
	ShaderBinderDesc.m_PerObjIndex = 0;
	ShaderBinderDesc.m_CameraIndex = 1;

	FPSODescriptor SkyboxPSODesc;
	SkyboxPSODesc.CullMode = ECullMode::CULL_MODE_FRONT;
	SkyboxPSODesc.DepthFunc = EComparisonFunc::LESS_EQUAL;
	SkyboxPSODesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/Skybox.hlsl", ShaderBinderDesc, ShaderDessc);
	FRenderer::GraphicFactroy->CreatePSO(PSO_SKYBOX, SkyboxPSODesc);
}
```

![1669541205789](image/README/1669541205789.png)
