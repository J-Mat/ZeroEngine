#pragma once

#include "Core.h"
#include "Core/Base/PublicSingleton.h"



namespace Zero
{
	enum class ETextureFormat
	{
		eDDS,
		eBMP,
		eJPG,
		ePNG,
		eTIFF,
		eGIF,
		eICO,
		eTGA,
		eHDR,
		ePIC,
		eNotSupported
	};

	class FTexture2D;
	class FMipsGenerator
	{
	public:
		FMipsGenerator() = default;
		void Add(Ref<FTexture2D> Texture);
		void Generate();
	private:
		std::vector<Ref<FTexture2D>> m_Resources;
	};

	class FTextureCube;
	class FTextureManager : public IPublicSingleton<FTextureManager>
	{
	public:
		void Tick();
		void LoadDefaultTexture();
		FTextureHandle LoadTexture(std::string const& FileName, bool bNeedMip = true);
		Ref<FTexture2D> GetTextureByHandle(FTextureHandle Handle);
		FTextureHandle GetDefaultTextureHandle() { return m_DefaultHandle; };
		Ref<FTexture2D> GetDefaultTexture() { return GetTextureByHandle(m_DefaultHandle); };
		Ref<FTextureCube> GetCurrentSkybox();
		Ref<FTextureCube> CreateTextureCubemap(const std::string& TextureName, std::string* FileNames);
	public:
		Ref<FTexture2D> GetLutTexture() { return m_LutTexture; };
	private:
		std::map<std::string, FTextureHandle> m_LoadedTexNameHandleMap{};
		//std::map<FTextureHandle, Ref<FTexture2D>> m_TextureMap{};
		std::map<FTextureHandle, Ref<FTexture2D>> m_TextureMap{};
		std::map<FTextureHandle, Ref<FTextureCube>> m_TextureCubeMap{};
		FMipsGenerator m_MipsGenrators;
	private:
		FTextureHandle LoadRegularTexture(std::string const& FileName, bool bNeedMip);
		FTextureHandle LoadTexture_HDR_TGA_PIC(std::string const& FileName);
	private:
		FTextureHandle m_DefaultHandle;
		FTextureHandle m_SkyboxHandle;
		Ref<FTexture2D> m_LutTexture;
	};
}
