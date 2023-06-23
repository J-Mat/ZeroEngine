#include "TextureManager.h"
#include "FileUtil.h"
#include "ZConfig.h"
#include "Image.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/Texture.h"

namespace Zero
{
	void FMipsGenerator::Add(Ref<FTexture2D> Texture)
	{
		m_Resources.push_back(Texture);
	}

	void FMipsGenerator::Generate()
	{
		for (auto Texture2D : m_Resources)
		{
			Texture2D->GenerateMip();
		}
		m_Resources.clear();
	}

	ETextureFormat GetTextureFormat(std::string const& path)
	{
		std::string extension = FileUtil::GetExtension(path);
		std::transform(std::begin(extension), std::end(extension), std::begin(extension), [](char c) {return std::tolower(c); });

		if (extension == ".dds")
			return ETextureFormat::eDDS;
		else if (extension == ".bmp")
			return ETextureFormat::eBMP;
		else if (extension == ".jpg" || extension == ".jpeg")
			return ETextureFormat::eJPG;
		else if (extension == ".png")
			return ETextureFormat::ePNG;
		else if (extension == ".tiff" || extension == ".tif")
			return ETextureFormat::eTIFF;
		else if (extension == ".gif")
			return ETextureFormat::eGIF;
		else if (extension == ".ico")
			return ETextureFormat::eICO;
		else if (extension == ".tga")
			return ETextureFormat::eTGA;
		else if (extension == ".hdr")
			return ETextureFormat::eHDR;
		else if (extension == ".pic")
			return ETextureFormat::ePIC;
		else
			return ETextureFormat::eNotSupported;
	}

    void FTextureManager::Tick()
    {
		m_MipsGenrators.Generate();
    }

	void FTextureManager::LoadDefaultTexture()
	{
		m_DefaultHandle = LoadTexture("Texture\\DefaultTexture.png");
		{
			auto Handle = LoadTexture("Texture\\pbr\\IBL_BRDF_LUT.png");
			m_LutTexture = GetTextureByHandle(Handle);
		}
	}


	FTextureHandle FTextureManager::LoadTexture(std::string const& FileName, bool bNeedMip)
    {
		if (FileName == "")
		{
			return FTextureHandle();
		}
		if (auto Iter = m_LoadedTexNameHandleMap.find(FileName); Iter != m_LoadedTexNameHandleMap.end())
		{
			return Iter->second;
		}

		ETextureFormat Format = GetTextureFormat(FileName);
		switch (Format)
		{
		case ETextureFormat::eDDS:
		case ETextureFormat::eBMP:
		case ETextureFormat::ePNG:
		case ETextureFormat::eJPG:
		case ETextureFormat::eTIFF:
		case ETextureFormat::eGIF:
		case ETextureFormat::eICO:
		case ETextureFormat::eTGA:
			return LoadRegularTexture(FileName, bNeedMip);
		case ETextureFormat::eHDR:
		case ETextureFormat::ePIC:
			return LoadTexture_HDR_TGA_PIC(FileName);
		case ETextureFormat::eNotSupported:
		default:
			CORE_ASSERT(false, "Unsupported Texture Format!");
		}
        return FTextureHandle();
    }

	Ref<FTexture2D> FTextureManager::GetTextureByHandle(FTextureHandle Handle)
	{
		if (auto Iter = m_TextureMap.find(Handle); Iter != m_TextureMap.end())
		{
			return Iter->second;
		}	
		return GetTextureByHandle(m_DefaultHandle);
	}


	Ref<FTextureCube> FTextureManager::GetCurrentSkybox()
	{
		if (auto Iter = m_TextureCubeMap.find(m_SkyboxHandle); Iter != m_TextureCubeMap.end())
		{
			return Iter->second;
		}	
		return nullptr;
	}

	Ref<FTextureCube> FTextureManager::CreateTextureCubemap(const std::string& TextureName, std::string* FileNames)
	{
		FTextureHandle Handles[6];
		for (size_t i = 0; i < 6; i++)
		{
			Handles[i] = LoadTexture(FileNames[i]);
		}
		m_SkyboxHandle = { TextureName, FIndexGetter<FTextureHandle>::Get() };
		Ref<FTextureCube> Texture = FDX12Device::Get()->GetOrCreateTextureCubemap(Handles, TextureName);
		m_TextureCubeMap.insert({ m_SkyboxHandle, Texture});
		return Texture;
	}

	FTextureHandle FTextureManager::LoadRegularTexture(std::string const& FileName, bool bNeedMip)
	{
		Ref<FTexture2D> Texture =  FGraphic::GetDevice()->GetOrCreateTexture2D(FileName, bNeedMip);
		FTextureHandle Handle{ FileName, FIndexGetter<FTextureHandle>::Get() };
		m_TextureMap.insert({ Handle ,Texture });
		m_LoadedTexNameHandleMap.insert({Handle.TextureName, Handle});
		if (bNeedMip)
		{
			m_MipsGenrators.Add(Texture);
		}
		return Handle;
	}

	FTextureHandle FTextureManager::LoadTexture_HDR_TGA_PIC(std::string const& FileName)
	{
		return {};
	}
}
