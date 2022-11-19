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
	class TLibrary
	{
	public:
		static void Push(const std::string& id, Ref<T> Instance);
		static void Update(const std::string& id, Ref<T> Instance);
		static Ref<T> Fetch(const std::string& id);
		static void Remove(const std::string& id);
		static void Reset();

		static std::unordered_map<std::string, Ref<T>> Mapper;
	};

	template<class T>
	void TLibrary<T>::Push(const std::string& ID, Ref<T> Instance)
	{
		if (Mapper.find(ID) == Mapper.end())
		{
			Mapper[ID] = Instance;
		}
	}
	
	template<class T>
	void TLibrary<T>::Update(const std::string& ID, Ref<T> Instance)
	{
		Mapper.insert({ ID, Instance });
	}

	template<class T>
	Ref<T> TLibrary<T>::Fetch(const std::string& ID)
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
	void TLibrary<T>::Remove(const std::string& ID)
	{
		if (Mapper.find(ID) != Mapper.end())
		{
			Mapper.erase(ID);
		}
	}

	template<class T>
	void TLibrary<T>::Reset()
	{
		Mapper.clear();
	}

	template<class T>
	std::unordered_map<std::string, Ref<T>> TLibrary<T>::Mapper;
}
