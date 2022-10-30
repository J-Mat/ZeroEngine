#pragma once
#include "Header.h"
#include <string>


namespace ZHT
{
	struct FParamElement
	{
		std::string Name;
		bool bConst = false;
		bool bPointer = false;
		bool bRefercence = false;
		bool bIsClass = false;
		std::string DataType;
		uint32_t LineIndex = 0;

		std::vector<FParamElement> InternelType;

		std::string Category = "Default";
	};

	struct FPropertyElement : public FParamElement
	{
		bool bStatic = false;
		std::string Value;
		std::set<std::string> Fields;
		std::map<std::string, std::string> Metas;
		FPropertyElement() = default;
	};

	struct FClassElement
	{
		std::string ClassName;
		std::string ClassTagName;
		std::vector<FPropertyElement> Properties;
		std::string DerivedName = "";
		uint32_t LineIndex;
		std::filesystem::path OriginFilePath;
		std::filesystem::path HeaderPath;
		std::filesystem::path CppPath;
	};

	struct FToken
	{
		FToken() = default;
		FToken(const std::string& _Str, int32_t _LineIndex)
			:TokenName(_Str)
			,LineIndex(_LineIndex)
		{
		}
		std::string TokenName = "";
		int32_t LineIndex = -1;
		
		bool IsValid()
		{
			return LineIndex >= 0;
		}
	};

	class FFileParser
	{
	public:
		FFileParser() = default;
		void ParseWhiteSpace();
		void AcceptSpecifiedToken(const std::string& Token);
		void AcceptQuotationToken(const char& Char);
		void AcceptValueUntilChar(const char EndChar);
		void AcceptComment();
		void AcceptStringOrChar();
		void AcceptDefaultToken();
		void Parse(std::filesystem::path Path);
		bool CheckNeedGenerateReflection();
		FToken GetType(uint32_t& TokenIndex, bool& bIsClass);
		void CollectMetaAndField(FPropertyElement& PropertyElement);
		void CollectProperty(FPropertyElement& PropertyElement);
		bool LocatePropertyTag();
		void CollectClassInfo(FClassElement& ClassElement);
		void LogClassInfo(FClassElement& ClassElement);
		void GenerateReflectionHeaderFile(FClassElement& ClassElement);

		bool IsDerived(std::string DerivedClass, std::string BasedClass);

		std::string WriteAddPropertyCode(const FClassElement& ClassElement);
		void GenerateReflectionCppFile(const FClassElement& ClassElement);
		std::string WriteGenerateActors(std::vector<std::string>& ActorClassNames);
		void WriteLinkReflectionFile(std::set<std::filesystem::path>& AllLinkCppFiles);
		void WriteInitActorGeneratedFile();
		
	private:
		std::map<std::string, std::string> m_DerivedRelation;
		std::vector<FClassElement> m_AllClassElements; 
		std::string m_Content;
		std::filesystem::path m_CurFilePath;
		size_t m_ContentSize;
		size_t m_CurPos;
		int32_t m_CurLineIndex; 
		std::vector<FToken> m_Tokens;
		size_t m_CurrentTokenIndex;
		size_t m_ClassTagIndex;
		size_t m_ClassBodyIndex;
		size_t m_PropertyIndex;
	};

}
