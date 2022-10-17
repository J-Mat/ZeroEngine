#pragma once
#include "Core.h"
#include "ObjectGenerator.h"


namespace Zero
{
	class UCoreObject;
	class UProperty;
	class FClassInfoCollection
	{
	public:
		FClassInfoCollection() = default;
		
		UProperty* AddProperty(const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize);

		template<class T>
		inline T* ConstructProperty(const std::string& PropertyName, void* Data, uint32_t PropertySize, const std::string& PropertyType);
		UCoreObject* m_Outer;
		UProperty* HeadProperty = nullptr;
		UProperty* TailProprty = nullptr;;
	};
}

