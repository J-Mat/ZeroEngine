#pragma once

#include "Core.h"
#include "RenderStage.h"


namespace Zero
{
	class FRenderTargetCube;
	class FRenderItem;
	class FForwardStage : public FRenderStage
	{
	public:
		PipeStageCreate(FForwardStage)
		FForwardStage() = default;
		virtual ~FForwardStage() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnDraw() override;
	private:
		void InitIBLRenderCube();
		void CreateIBLIrradianceMap();
	private:
		Ref<FRenderItem> m_IBLCubeRenderItem;
		Ref<FRenderTargetCube> m_RenderTargetCube = nullptr;
		bool m_bGenerateIrradianceMap = false;
	};
}