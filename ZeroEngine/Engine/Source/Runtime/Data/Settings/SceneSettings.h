#pragma once

#include "Settings.h"
#include "SceneSettings.reflection.h"

namespace Zero
{
	UCLASS()
	class USceneSettings : public USettings
	{
		GENERATED_BODY()
	public:
		USceneSettings() = default;
	public:
		UPROPERTY()
		bool m_bUseSkyBox = false;

		UPROPERTY()
		FTextureHandle m_SkyBoxRright;

		UPROPERTY()
		FTextureHandle m_SkyBoxLeft;

		UPROPERTY()
		FTextureHandle m_SkyBoxUp;

		UPROPERTY()
		FTextureHandle m_SkyBoxDown;

		UPROPERTY()
		FTextureHandle m_SkyBoxFront;

		UPROPERTY()
		FTextureHandle m_SkyBoxBack;
	};
}
