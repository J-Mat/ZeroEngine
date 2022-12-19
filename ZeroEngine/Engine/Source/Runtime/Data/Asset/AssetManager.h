#pragma once
#include "Core/Base/PublicSingleton.h"
#include "AssetObject/Asset.h"
#include "World/Object/ObjectGenerator.h"
#include "Render/RHI/Texture.h"
#include "Render/RendererAPI.h"

namespace Zero
{
	class FAssetManager : public IPublicSingleton<FAssetManager>
	{
	public:
		template<typename T, typename ... Args>
		T* NewAsset(std::string AssetName, Args&& ... args)
		{
			T* Asset = CreateObject<T>(nullptr, std::forward<Args>(args)...);
			FAssetHandle Handle = Utils::newGuid();
			Asset->SetAssetName(AssetName);
			Asset->SetAssetHandle(Handle);
			Push(AssetName, Asset);
			return Asset;
		}
		
		void Push(const std::string& id, UAsset* Asset);
		
		template<class T>
		T* Fetch(const std::string& ID)
		{
			auto Iter = m_AllAssets.find(ID);
			if (Iter != m_AllAssets.end())
			{
				return dynamic_cast<T*>(Iter->second);
			}
			return LoadAsset<T>(ID);
		};

		UAsset* Fetch(const std::string& ID)
		{
			auto Iter = m_AllAssets.find(ID);
			if (Iter != m_AllAssets.end())
			{
				return Iter->second;
			}
			return nullptr;
		};

		Ref<FTexture2D> FetchTexture(const std::string& ID)
		{
			return RHI_Factory->GetOrCreateTexture2D(ID);
		}

		template<class T>
		T* Remove(const std::string& ID)
		{
			auto Iter = m_AllAssets.find(ID);
			if (Iter != m_AllAssets.end())
			{
				UAsset* Asset = Iter->second;
				m_AllAssets.erase(Iter);
				return dynamic_cast<T*>(Asset);
			}
			return nullptr;
		}

		UAsset* Remove(const std::string& ID)
		{
			auto Iter = m_AllAssets.find(ID);
			if (Iter != m_AllAssets.end())
			{
				UAsset* Asset = Iter->second;
				m_AllAssets.erase(Iter);
				return Asset;
			}
			return nullptr;
		}
		
		template<class T>
		T* LoadAsset(std::filesystem::path Path)
		{
			UAsset* Asset = Fetch(Path.string());
			if (Asset == nullptr)
			{
				Asset = Deserialize(ZConfig::GetAssestsFullPath(Path.string()));
			}
			T* SpecifiedAsset = dynamic_cast<T*>(Asset);
			if (SpecifiedAsset != nullptr)
			{
				Push(Path.string(), Asset);
				return SpecifiedAsset;
			}
			else if (Asset != nullptr)
			{
				delete Asset;	
			}
			return nullptr;
		}
		
		void Serialize(UAsset* Asset);
		void Serialize(std::string AssetName);
		UAsset* Deserialize(std::filesystem::path Path);



	private:
		std::map<std::string, UAsset*> m_AllAssets;
	};

}