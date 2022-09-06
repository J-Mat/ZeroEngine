#pragma onces
#include "Core.h"

namespace Zero
{
	class FPipelineStage
	{
	public:
		FPipelineStage(const std::string& Iname = "Stage")
			: m_Name(Iname)
		{}
		virtual ~FPipelineStage() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnDraw() {}
	protected:
		std::string m_Name;
	};
}
