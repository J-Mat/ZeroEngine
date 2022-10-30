#pragma once
#include "Core.h"
#include "ObjectGenerator.h"


namespace Zero
{
	class UCoreObject;
	class UProperty;
	class UVariableProperty;
	class UClassProperty;
	class FClassInfoCollection
	{
	public:
		FClassInfoCollection() = default;
		
		UVariableProperty* AddVariableProperty(const std::string& ClassName,  const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize);

		UClassProperty* AddClassProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize);
		
		UProperty* FindProperty(const std::string PropertyName);

		template<class T, typename ... Args>
		inline T* ConstructProperty(Args&& ... args);
		
		

		bool FindMetas(const std::string Key, std::string& Value);
		bool HasField(const std::string Field);

		void AddMeta(const std::string& PropertyName, const std::string& Key, const std::string& Value);
		void AddMeta(const std::string& Key, const std::string& Value);
		
		void AddField(const std::string& PropertyName, const std::string& Fields);
		void AddField(const std::string& Field);
		UCoreObject* m_Outer;
		UProperty* HeadProperty = nullptr;
		UProperty* TailProprty = nullptr;
	private:
#ifdef EDITOR_MODE
		std::set<std::string> m_Fields;
		std::map<std::string, std::string> m_Metas;
#endif
	};
}

