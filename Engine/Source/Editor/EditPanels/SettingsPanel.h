#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"

namespace Zero
{
	class FSettingsPanel : public FBasePanel
	{
	public:
		FSettingsPanel() = default;
		virtual void OnGuiRender() override;
	};
}