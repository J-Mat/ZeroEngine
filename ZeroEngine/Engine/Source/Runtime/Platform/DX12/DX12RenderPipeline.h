#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "DX12Device.h"



namespace Zero
{
	class FDX12Device;
	class FDX12RenderPipeline : public IPublicSingleton<FDX12RenderPipeline>
	{
	public:
		FDX12RenderPipeline();

		void DrawFrame(Ref<FDX12Device> Device);
	};
}