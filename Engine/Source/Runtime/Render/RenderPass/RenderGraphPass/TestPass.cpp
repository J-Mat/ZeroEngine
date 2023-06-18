#include "TestPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RenderUtils.h"
#include "Render/Moudule/Material.h"
#include "Render/Moudule/ImageBasedLighting.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "World/LightManager.h"
#include "Render/Moudule/PSOCache.h"
#include "Render/Moudule/SceneCapture.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Render/RHI/CommandList.h"


namespace Zero
{
	FTestPass::FTestPass()
	{
	}

	void FTestPass::AddPass(FRenderGraph& RenderGraph)
	{
		std::string PassName = "ComputeShaderTest";
		struct FComputeShaderData
		{
			//FRGBufferReadOnlyID ReadBufferID;
			FRGBufferReadWriteID ResultBufferID;
			FRGBufferReadOnlyID ReadBackBufferID;
		};
		RenderGraph.AddPass<FComputeShaderData>(
			PassName.c_str(),
			[=](FComputeShaderData& Data, FRenderGraphBuilder& Builder)
			{
				/*
				{
					FRGBufferDesc Desc{
						.Stride = sizeof(float),
						.Size = sizeof(float) * 2048,
						.ResourceUsage = EResourceUsage::Default,
						.MiscFlags = EBufferMiscFlag::BufferRaw,
						.BindFlag = EResourceBindFlag::ShaderResource,
						.Format = EResourceFormat::R32_FLOAT
					};
					Builder.DeclareBuffer(RGResourceName::BufferArray, Desc);
					Data.ReadBufferID = Builder.ReadBuffer(RGResourceName::BufferArray);
				}
				*/
				
				{
					FRGBufferDesc Desc{
						.Stride = sizeof(float),
						.Size = sizeof(float) * 32,
						.ResourceUsage = EResourceUsage::Default,
						.MiscFlags = EBufferMiscFlag::BufferRaw,
						.BindFlag = EResourceBindFlag::UnorderedAccess,
						.Format = EResourceFormat::R32_TYPELESS
					};
					Builder.DeclareBuffer(RGResourceName::ResultBuffer, Desc);
					Data.ResultBufferID = Builder.WriteBuffer(RGResourceName::ResultBuffer);
				}

				Data.ReadBackBufferID = Builder.ReadBuffer(RGResourceName::ReadBackBuffer);

			},
			[=](const FComputeShaderData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				Ref<FComputeRenderItemPool> ComputeRenderItemPool = UWorld::GetCurrentWorld()->GetComputeRenderItemPool();
				Ref<FComputeRenderItem> Item = ComputeRenderItemPool->Request();
				Item->SetPsoID(EComputePsoID::PrefixSumTex);
				Item->PreDispatch(CommandListHandle);
				Ref<FShaderParamsGroup> ShaderParamsBinder = Item->GetShaderParamsGroup();
				/*
				FBuffer* InputBuffer = Context.GetBuffer(Data.ReadBufferID.GetResourceID());
				std::vector<float> ReadArray;
				for (size_t i = 0; i < 2048; ++i)
				{
					ReadArray.push_back(1.0f);
				}
				InputBuffer->Update(CommandListHandle, ReadArray.data(), sizeof(float) * ReadArray.size());


				ShaderParamsBinder->SetBuffer("gInputA", InputBuffer);
				*/

				FBuffer* OutputBuffer = Context.GetBuffer(Data.ResultBufferID.GetResourceID());
				ShaderParamsBinder->SetBuffer_Uav("gOutput", OutputBuffer);

				Item->Dispatch(CommandListHandle, 1, 1, 1);

				
				FBuffer* ReadBackBuffer = Context.GetBuffer(Data.ReadBackBufferID.GetResourceID());
				Ref<FCommandList> CommandList = FGraphic::GetDevice()->GetRHICommandList(CommandListHandle);
				CommandList->CopyResource(ReadBackBuffer->GetNative(), OutputBuffer->GetNative());	
				CommandList->TransitionBarrier(ReadBackBuffer->GetNative(), EResourceState::Common);
			},
			ERenderPassType::Compute,
			ERGPassFlags::ForceNoCull
		);
	}
}
