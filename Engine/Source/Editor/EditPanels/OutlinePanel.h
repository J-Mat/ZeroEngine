#pragma once

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