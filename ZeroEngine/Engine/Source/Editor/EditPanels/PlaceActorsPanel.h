#pragma once

#include <ZeroEngine.h>


namespace Zero
{
	struct FPlaceActorItem
	{
		Ref<FTexture2D> IconTexture;
		std::string ActorName;	
	};
	class FPlaceActorsPanel
	{
	public:
		FPlaceActorsPanel();	
		void OnGuiRender();
		void Init();
	private:
		std::vector<FPlaceActorItem> m_ActroItems;
	};
}