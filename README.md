# ZeroEngine

- [LearningDX12](https://github.com/J-Mat/ZeroEngine/tree/main/LearningDX12 "LearningDX12")  龙书代码，用cmake整理在一起
- [ZeroEngine](https://github.com/J-Mat/ZeroEngine/tree/main/ZeroEngine "ZeroEngine") 基于DX12的渲染器

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
FPSODescriptor SkyboxPSODesc;
SkyboxPSODesc.CullMode = ECullMode::CULL_MODE_FRONT;
SkyboxPSODesc.DepthFunc = EComparisonFunc::LESS_EQUAL;
SkyboxPSODesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader/Skybox.hlsl", ShaderBinderDesc, ShaderDessc);
FRenderer::GraphicFactroy->CreatePSO(PSO_SKYBOX, SkyboxPSODesc);
```

![1669541205789](image/README/1669541205789.png)

### Shader Code Reflecion 2022-12-9

I just used **ID3D12ShaderReflection** to get shader code layout

```cpp
ID3D12ShaderReflection* Reflection = nullptr;
D3DReflect(PassBlob->GetBufferPointer(), PassBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&Reflection);

D3D12_SHADER_DESC D3DShaderDesc;
Reflection->GetDesc(&D3DShaderDesc);
```

After getting the buffer and resource layout, take these info to cpp, and make these reflected to UI panel through code reflection in cpp. So we can edit the parameters easily.

| Shader Code                                                                                        | UI                                             |
| -------------------------------------------------------------------------------------------------- | ---------------------------------------------- |
| ![1670598404551](image/README/1670598404551.png)<br />![1670598425336](image/README/1670598425336.png) | ![1670599123303](image/README/1670599123303.png) |
