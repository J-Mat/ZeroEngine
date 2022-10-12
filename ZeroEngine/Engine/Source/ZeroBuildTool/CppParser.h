#pragma once
#include "Header.h"
#include <string>


namespace ZBT
{
	enum  EDataType
	{
		DT_Int32,
		DT_UInt32,
		DT_Float,
		DT_Double,
		DT_String,
		DT_Vector
	};

	struct FParamElement
	{
		std::string Name;
		bool bConst = false;
		bool bPointer = false;
		bool bRefercence = false;
		EDataType DataType;

		std::vector<FParamElement> InternelType;

		std::string Category = "Default";
	};

	struct FVariableElement : public FParamElement
	{
		bool bStatic = false;
		FVariableElement() = default;
	};

	struct FClassElement
	{
		std::string ClassName;
		std::vector<FVariableElement> Variables;
		std::filesystem::path Path;

		std::string GetPath() { return Path.string(); }
		std::string GetFileName() { return Path.stem().string(); }
	};

	struct FToken
	{
		FToken() = default;
		FToken(const std::string& _Str, uint32_t _LineIndex)
			:TokenName(_Str)
			,LineIndex(_LineIndex)
		{
		}
		std::string TokenName;
		uint32_t LineIndex = 0;
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
		void GenerateCodeReflectionFile();
	private:
		std::string m_Content;
		std::filesystem::path m_CurFilePath;
		size_t m_ContentSize;
		size_t m_CurPos;
		uint32_t m_CurLineIndex; 
		std::vector<FToken> m_Tokens;
	};

}
