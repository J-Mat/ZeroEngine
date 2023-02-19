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
	protected:
		std::string m_Name = "";
	};
	
#define PipeStageCreate(Type)    static Ref<FRenderStage> Create()\
							{\
								Ref<FRenderStage> pipe;\
								pipe.reset(new Type());\
								return pipe;\
							}
}
