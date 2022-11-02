#pragma once

#include <ZeroEngine.h>

#define ASSEST_PANEL_OBJ "Assests_Obj"
#define PLACEOBJ_PANEL "PlaceActor"

namespace Zero
{
	class FEditor
	{
	public:
		static Ref<FTexture2D> CreateIcon(const std::string&& FileName)
		{
			Ref<FImage> Image = CreateRef<FImage>(ZConfig::GetEditorContentFullPath(FileName).string());
			auto Icon = FRenderer::GraphicFactroy->CreateTexture2D(Image);
			Icon->RegistGuiShaderResource();
			return Icon;
		}
		static UActor* SelectedActor;
	};
}