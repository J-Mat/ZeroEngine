#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderTarget2D;
	class FRenderStage
	{
	public:
		FRenderStage(const std::string& Iname = "Stage")
			: m_Name(Iname)
		{}
		virtual ~FRenderStage() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnDraw() = 0;
		Ref<FRenderTarget2D> GetOutput() { return m_RenderTarget; }
		void SetInput(Ref<FRenderTarget2D> RenderTarget) { m_RenderTarget = RenderTarget; };
	protected:
		Ref<FRenderTarget2D> m_RenderTarget;
		std::string m_Name = "";
	};
	
#define PipeStageCreate(Type)    static Ref<FRenderStage> Create()\
							{\
								Ref<FRenderStage> pipe;\
								pipe.reset(new Type());\
								return pipe;\
							}
}
