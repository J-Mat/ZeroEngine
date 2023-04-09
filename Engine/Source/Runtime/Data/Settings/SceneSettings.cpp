#include "SceneSettings.h"
#include "StringUtils.h"
#include "Render/RendererAPI.h"
#include "World/World.h"
#include "World/Actor/SkyActor.h"


namespace Zero
{
	void USceneSettings::PostInit()
	{
		Supper::PostInit();
		GenrerateTextureCubemapHanle();
	}

	void USceneSettings::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		GenrerateTextureCubemapHanle();
		if (Property->GetPropertyName() == "m_bUseSkyBox")
		{
		}
	}

	USkyActor* USceneSettings::GetSkyActor()
	{
		USkyActor* SkyActor = UWorld::GetCurrentWorld()->GetSkyActor();
		if (SkyActor != nullptr)
		{
			SkyActor->SetPosition({0.0f, 0.0f, 0.0f});
			return SkyActor;
		}
		return UWorld::GetCurrentWorld()->CreateActor<USkyActor>();
	}

	void USceneSettings::GenrerateTextureCubemapHanle()
	{
		return;
		std::vector<std::string> Tokens = { m_SkyBoxRright.TextureName, m_SkyBoxLeft.TextureName, m_SkyBoxTop.TextureName, m_SkyBoxBottom.TextureName, m_SkyBoxFront.TextureName, m_SkyBoxBack.TextureName };
		std::string TempHandle = Utils::StringUtils::Join(Tokens, "|");
		if (m_TextureCubemapHandle != TempHandle)
		{
			m_TextureCubemapHandle = TempHandle;
			FTextureHandle Handles[CUBEMAP_TEXTURE_CNT] = { m_SkyBoxRright, m_SkyBoxLeft, m_SkyBoxTop, m_SkyBoxBottom, m_SkyBoxFront, m_SkyBoxBack };
			Ref<FTextureCubemap> TextureCubemap = FGraphic::GetDevice()->GetOrCreateTextureCubemap(Handles, "default");
			GetSkyActor()->SetTextureCubemap("gSkyboxMap", TextureCubemap);
		}
	}
}
