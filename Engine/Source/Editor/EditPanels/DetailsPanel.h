#pragma once
#include "World/Object/CoreObject.h"
#include "World/Object/ClassObject.h"
#include "World/Object/VariateProperty.h"
#include "World/Object/ClassProperty.h"
#include "World/Object/ObjectGlobal.h"
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