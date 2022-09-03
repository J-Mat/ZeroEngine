#pragma once

#include "Core.h"

namespace SIByL
{
	static const std::string AssetRoot = "..\\Assets\\";

	static std::string GetPathID(std::string pathortho)
	{
		replace(pathortho.begin(), pathortho.end(), '/', '\\');
		std::string id = "FILE=" + pathortho;
		return id;
	}

	template<class T>
	class Library
	{
	public:
		static void Push(const std::string& id, Ref<T> instance);
		static Ref<T> Fetch(const std::string& id);
		static void Remove(const std::string& id);
		static void Reset();

		static std::unordered_map<std::string, Ref<T>> Mapper;
	};

	template<class T>
	void Library<T>::Push(const std::string& id, Ref<T> instance)
	{
		if (Mapper.find(id) == Mapper.end())
		{
			Mapper[id] = instance;
		}
	}

	template<class T>
	Ref<T> Library<T>::Fetch(const std::string& id)
	{
		if (Mapper.find(id) != Mapper.end())
		{
			return Mapper[id];
		}
		else
		{
			return nullptr;
		}
	}

	template<class T>
	void Library<T>::Remove(const std::string& id)
	{
		if (Mapper.find(id) != Mapper.end())
		{
			Mapper.erase(id);
		}
	}

	template<class T>
	void Library<T>::Reset()
	{
		Mapper.clear();
	}

	template<class T>
	std::unordered_map<std::string, Ref<T>> Library<T>::Mapper;

	class Texture2D;
	template< >
	Ref<Texture2D> Library<Texture2D>::Fetch(const std::string& id);

	class TextureCubemap;
	template< >
	Ref<TextureCubemap> Library<TextureCubemap>::Fetch(const std::string& id);

}