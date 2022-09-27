#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "DX12Device.h"
#include "Core/Framework/LayerStack.h"



namespace Zero
{
	class IDevice;
	class FDX12RenderPipeline : public IPublicSingleton<FDX12RenderPipeline>
	{
	public:
		FDX12RenderPipeline();

		void DrawFrame();
	private:
		FLayerStack m_RenderLayerStack;
	};
}