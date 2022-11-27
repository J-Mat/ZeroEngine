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
		std::vector<std::string> Tokens = { m_SkyBoxRright, m_SkyBoxLeft, m_SkyBoxTop, m_SkyBoxBottom, m_SkyBoxFront, m_SkyBoxBack };
		std::string TempHandle = Utils::StringUtils::Join(Tokens, "|");
		if (m_TextureCubemapHandle != TempHandle)
		{
			m_TextureCubemapHandle = TempHandle;
			FTextureHandle Handles[CUBEMAP_TEXTURE_CNT] = { m_SkyBoxRright, m_SkyBoxLeft, m_SkyBoxTop, m_SkyBoxBottom, m_SkyBoxFront, m_SkyBoxBack };
			Ref<FTextureCubemap> TextureCubemap = FRenderer::GraphicFactroy->GetOrCreateTextureCubemap(Handles, "default");
			GetSkyActor()->SetTextureCubemap("gSkyboxMap", TextureCubemap);
		}
	}
}
