#pragma once
#include "RenderGraphResourceName.h"
#include "RenderGraphPass.h"
#include "RenderGraphContext.h"
#include "Render/ResourceCommon.h"

namespace Zero
{
	struct FRGTextureDesc
	{
		ETextureType Type = ETextureType::TextureType_2D;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Depth = 0;
		uint32_t ArraySize = 1;
		uint32_t MipLevels = 1;
		uint32_t SampleCount = 1;
		EResourceUsage HeapType = EResourceUsage::Default;
		FTextureClearValue ClearValue{};
		EResourceFormat Format = EResourceFormat::UNKNOWN;
	};
	struct FRGBufferDesc
	{
		uint64_t Size = 0;
		uint32_t Stride = 0;
		EResourceUsage ResourceUsage = EResourceUsage::Default;
		EBufferMiscFlag MiscFlags = EBufferMiscFlag::None;
		EResourceFormat Format = EResourceFormat::UNKNOWN;
	};

	static void FillTextureDesc(FRGTextureDesc const& RGTexDesc, FTextureDesc& TexDesc)
	{
		TexDesc.Type = RGTexDesc.Type;
		TexDesc.Width = RGTexDesc.Width;
		TexDesc.Height = RGTexDesc.Height;
		TexDesc.Depth = RGTexDesc.Depth;
		TexDesc.ArraySize = RGTexDesc.ArraySize;
		TexDesc.MipLevels = RGTexDesc.MipLevels;
		TexDesc.Format = RGTexDesc.Format;
		TexDesc.SampleCount = RGTexDesc.SampleCount;
		TexDesc.HeapType = RGTexDesc.HeapType;
		TexDesc.ClearValue = RGTexDesc.ClearValue;
		TexDesc.BindFlags = EResourceBindFlag::None;
		TexDesc.InitialState = EResourceState::Common;
	}
}