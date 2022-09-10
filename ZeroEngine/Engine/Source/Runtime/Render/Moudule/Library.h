#pragma once

#include "Core.h"

namespace Zero
{

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
		static void Push(const std::string& id, Ref<T> Instance);
		static Ref<T> Fetch(const std::string& id);
		static void Remove(const std::string& id);
		static void Reset();

		static std::unordered_map<std::string, Ref<T>> Mapper;
	};

	template<class T>
	void Library<T>::Push(const std::string& ID, Ref<T> Instance)
	{
		if (Mapper.find(ID) == Mapper.end())
		{
			Mapper[ID] = Instance;
		}
	}

	template<class T>
	Ref<T> Library<T>::Fetch(const std::string& ID)
	{
		if (Mapper.find(ID) != Mapper.end())
		{
			return Mapper[ID];
		}
		else
		{
			return nullptr;
		}
	}

	template<class T>
	void Library<T>::Remove(const std::string& ID)
	{
		if (Mapper.find(ID) != Mapper.end())
		{
			Mapper.erase(ID);
		}
	}

	template<class T>
	void Library<T>::Reset()
	{
		Mapper.clear();
	}
}
