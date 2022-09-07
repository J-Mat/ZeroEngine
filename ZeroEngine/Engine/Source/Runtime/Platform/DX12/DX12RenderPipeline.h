#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "DX12Device.h"
#include "Core/Framework/LayerStack.h"



namespace Zero
{
	class FDX12Device;
	class FDX12RenderPipeline : public IPublicSingleton<FDX12RenderPipeline>
	{
	public:
		FDX12RenderPipeline();

		void DrawFrame();
		void SetDevice(Ref<FDX12Device> Device);
	private:
		FLayerStack m_RenderLayerStack;
		Ref<FDX12Device> m_Device;
	};
}