#pragma once

#include <ZeroEngine.h>
#include "EditPanels/BasePanel.h"


namespace Zero
{
	class FDetailPanel : public FBasePanel
	{
	public:
		FDetailPanel();
		virtual void OnGuiRender() override;
	};
}