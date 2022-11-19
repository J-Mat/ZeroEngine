#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"

namespace Zero
{
	class FOutlinePanel : public FBasePanel
	{
	public:
		FOutlinePanel() = default;
		virtual void OnGuiRender() override;
	};
}