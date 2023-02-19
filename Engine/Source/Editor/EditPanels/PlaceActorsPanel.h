#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"

namespace Zero
{
	struct FPlaceActorItem
	{
		Ref<FTexture2D> IconTexture;
		std::string ActorName;	
	};
	class FPlaceActorsPanel : public FBasePanel
	{
	public:
		FPlaceActorsPanel() = default;
		virtual	void OnGuiRender() override;
		virtual void Init();
	private:
		std::vector<FPlaceActorItem> m_ActroItems;
	};
}