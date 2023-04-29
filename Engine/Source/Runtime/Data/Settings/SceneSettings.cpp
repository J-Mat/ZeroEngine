#include "SceneSettings.h"
#include "StringUtils.h"
#include "Render/RendererAPI.h"
#include "World/World.h"
#include "World/Actor/SkyActor.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "Render/RenderUtils.h"
#include "Render/Moudule/PSOCache.h"


namespace Zero
{
	void USceneSettings::PostInit()
	{
		Supper::PostInit();
		GenrerateTextureCubemapHandle();
	}

	void USceneSettings::PostEdit(UProperty* Property)
	{
		Supper::PostEdit(Property);
		if (Property->GetPropertyName() == "m_bUseSkyBox" && m_bUseSkyBox)
		{
			GenrerateTextureCubemapHandle();
		}
		if (Property->GetPropertyName() == "m_ShadowType")
		{	
			FPSOCache::Get().RegsiterForwardLitPSO();
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

	void USceneSettings::GenrerateTextureCubemapHandle()
	{
		std::vector<std::string> Tokens = { m_SkyBoxRright.TextureName, m_SkyBoxLeft.TextureName, m_SkyBoxTop.TextureName, m_SkyBoxBottom.TextureName, m_SkyBoxFront.TextureName, m_SkyBoxBack.TextureName };
		std::string TempHandle = Utils::StringUtils::Join(Tokens, "|");
		if (m_TextureCubemapHandle != TempHandle)
		{
			m_TextureCubemapHandle = TempHandle;
			std::string* Handles = Tokens.data();
			Ref<FTextureCube> TextureCubemap = FTextureManager::Get().CreateTextureCubemap("skybox", Handles);
			if (TextureCubemap != nullptr)
			{
				FRenderUtils::SetNeedRefreshIBL();
			}
			GetSkyActor()->SetTextureCubemap("gSkyboxMap", TextureCubemap);
		}
	}
}
