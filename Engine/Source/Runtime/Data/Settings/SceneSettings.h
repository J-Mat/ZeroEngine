#pragma once

#include "Settings.h"
#include "SceneSettings.reflection.h"
#include "Render/Moudule/Texture/TextureManager.h"

namespace Zero
{
	class USkyActor;
	UCLASS()
	class USceneSettings : public USettings
	{
		GENERATED_BODY()
	public:
		USceneSettings() = default;
		virtual void PostInit() override;
		virtual void PostEdit(UProperty* Property) override;
	public:
		UPROPERTY()
		bool m_bUseSkyBox = false;

		UPROPERTY()
			FTextureHandle m_SkyBoxRright; //= FTextureManager::Get().LoadTexture("Texture/skybox/right.jpg");//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
			FTextureHandle m_SkyBoxLeft; //= FTextureManager::Get().LoadTexture("Texture/skybox/left.jpg");

		UPROPERTY()
			FTextureHandle m_SkyBoxTop; //= FTextureManager::Get().LoadTexture("Texture/skybox/top.jpg");//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
			FTextureHandle m_SkyBoxBottom; //= FTextureManager::Get().LoadTexture("Texture/skybox/bottom.jpg");//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
			FTextureHandle m_SkyBoxFront; //= FTextureManager::Get().LoadTexture("Texture/skybox/front.jpg");//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
			FTextureHandle m_SkyBoxBack; //= FTextureManager::Get().LoadTexture("Texture / skybox / back.jpg");//DEFAULT_TEXTURE_HANDLE;
	private: 
		void GenrerateTextureCubemapHanle();
		USkyActor* GetSkyActor();
	private:
		std::string m_TextureCubemapHandle = "";
	};
}
