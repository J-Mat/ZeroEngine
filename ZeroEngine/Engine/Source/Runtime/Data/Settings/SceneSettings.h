#pragma once

#include "Settings.h"
#include "SceneSettings.reflection.h"

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
		FTextureHandle m_SkyBoxRright = "Texture/skybox/right.jpg";//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
		FTextureHandle m_SkyBoxLeft = "Texture/skybox/left.jpg";;

		UPROPERTY()
		FTextureHandle m_SkyBoxTop = "Texture/skybox/top.jpg";//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
		FTextureHandle m_SkyBoxBottom = "Texture/skybox/bottom.jpg";//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
		FTextureHandle m_SkyBoxFront = "Texture/skybox/front.jpg";//DEFAULT_TEXTURE_HANDLE;

		UPROPERTY()
		FTextureHandle m_SkyBoxBack = "Texture/skybox/back.jpg";//DEFAULT_TEXTURE_HANDLE;
	private: 
		void GenrerateTextureCubemapHanle();
		USkyActor* GetSkyActor();
	private:
		std::string m_TextureCubemapHandle = "";
	};
}
