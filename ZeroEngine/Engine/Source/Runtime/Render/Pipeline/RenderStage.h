#pragma onces
#include "Core.h"
//#include "Render/RHI/RenderTarget.h"

namespace Zero
{
	class FRenderTarget;
	class FRenderStage
	{
	public:
		FRenderStage(const std::string& Iname = "Stage")
			: m_Name(Iname)
		{}
		virtual ~FRenderStage() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnDraw() {}
		Ref<FRenderTarget> GetOutput() { return m_RenderTarget; }
		void SetInput(Ref<FRenderTarget> RenderTarget) { m_RenderTarget = RenderTarget; };
	protected:
		Ref<FRenderTarget> m_RenderTarget;
		std::string m_Name = "";
	};
}
