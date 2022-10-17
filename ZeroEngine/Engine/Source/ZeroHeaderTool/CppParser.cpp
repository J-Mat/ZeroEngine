#include "CppParser.h"
#include "StringUtils.h"
#include "Log.h"
#include <string>
#include "Config.h"
#include "Utils.h"


#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISLETTER(ch)        (((ch) >= 'a' && (ch) <= 'z') ||  ((ch) >= 'A' && (ch) <= 'Z'))
#define ISLETTER_OR_DIGIT(ch)  (ISDIGIT(ch) || ISLETTER(ch))
#define CUR_CHAR m_Content[m_CurPos] 
#define NEXT_CHAR m_Content[m_CurPos + 1] 
#define PRE_CHAR m_Content[m_CurPos - 1] 
#define INCREASE_INDEX m_CurPos++

#define PUSH_TO_CONTENT  \
Contents.push_back(Stream.str()); \
Stream.str("");

namespace ZHT
{
	void FFileParser::ParseWhiteSpace()
	{
		while (CUR_CHAR == ' ' || CUR_CHAR == '\t' || CUR_CHAR == '\n' || CUR_CHAR == '\r')
		{
			if (CUR_CHAR == '\n')
			{
				m_CurLineIndex++;
			}
			m_CurPos++;
		}
	}

	void FFileParser::AcceptSpecifiedToken(const std::string& TokenName)
	{
		uint32_t i = 0;
		while (i < TokenName.size())
		{
			CLIENT_ASSERT(m_CurPos < m_Content.size(), "Current Pos is out of range!");
			CLIENT_ASSERT(CUR_CHAR == TokenName[i], "Accept Token Failed");
			INCREASE_INDEX;
		}
		m_Tokens.push_back({ TokenName, m_CurLineIndex });
	}

	void FFileParser::AcceptQuotationToken(const char& LeftChar)
	{
		std::stringstream Steam;
		char RightChar = 0;
		int StackValue = 1;
		switch (LeftChar)
		{
		case '<':
			RightChar = '>';
			break;
		case '(':
			RightChar = ')';
			break;
		case '[':
			RightChar = ']';
			break;
		default:
			CLIENT_ASSERT(false, "Invalid Quotation");
			break;
		}

		Steam << CUR_CHAR;
		INCREASE_INDEX;
		CLIENT_ASSERT(m_CurPos < m_Content.size(), "Current Pos is out of range!");
		for (;;)
		{ 
			Steam << CUR_CHAR;
			if (CUR_CHAR == LeftChar)
			{
				StackValue++;
			}
			else if (CUR_CHAR == RightChar)
			{ 
				StackValue--;
				if (StackValue == 0)
				{
					INCREASE_INDEX;
					break;
				}
			}
			INCREASE_INDEX;
		}
		m_Tokens.push_back({ Steam.str(), m_CurLineIndex });
	}

	void FFileParser::AcceptValueUntilChar(const char EndChar)
	{
		ParseWhiteSpace();
		std::stringstream Steam;
		for (;;)
		{ 
			if (CUR_CHAR == EndChar)
			{ 
				INCREASE_INDEX;
				break;
			}
			Steam << CUR_CHAR;
			INCREASE_INDEX;
		}
		
	}

	void FFileParser::AcceptComment()
	{
		if (CUR_CHAR == '/')
		{
			AcceptValueUntilChar('\n');
		}
		else if (CUR_CHAR == '*')
		{
			INCREASE_INDEX;
			CLIENT_ASSERT(m_CurPos < m_Content.size(), "Current Pos is out of range!");
			AcceptValueUntilChar('*'); 
			if (CUR_CHAR == '/')
			{
				INCREASE_INDEX;
				return;
			}
		}
	}

	void FFileParser::AcceptStringOrChar()
	{
		std::stringstream Stream;
		char Quotation = CUR_CHAR;
		Stream << CUR_CHAR;
		INCREASE_INDEX;
		for(;;)
		{
			Stream << CUR_CHAR;
			if (CUR_CHAR == Quotation && PRE_CHAR != '\\')
			{
				INCREASE_INDEX;
				break;
			}
			INCREASE_INDEX;
		}
		m_Tokens.push_back({ Stream.str(), m_CurLineIndex });
	}


	void FFileParser::AcceptDefaultToken()
	{
		std::stringstream Stream;
		for (;;)
		{ 
			if (ISLETTER_OR_DIGIT(CUR_CHAR) || CUR_CHAR == '_')
			{ 
				Stream << CUR_CHAR;
				INCREASE_INDEX;
			}
			else if (CUR_CHAR == ':' && NEXT_CHAR == ':')
			{
				Stream << CUR_CHAR;
				INCREASE_INDEX;
				Stream << CUR_CHAR;
				INCREASE_INDEX;
			}
			else
			{
				break;
			}
		}
		if (Stream.rdbuf()->in_avail() == 0)
		{
			Stream << CUR_CHAR;
			INCREASE_INDEX;
		}
		m_Tokens.push_back({ Stream.str(), m_CurLineIndex });
	}

	void FFileParser::Parse(std::filesystem::path Path)
	{
		m_CurFilePath = Path;
		m_Content = Zero::StringUtils::ReadFile(Path.string());
		m_CurPos = 0;
		m_ContentSize = m_Content.size();
		m_Tokens.clear();
		m_CurLineIndex = 0;
		ParseWhiteSpace();
		while (m_CurPos < m_ContentSize)
		{
			switch (CUR_CHAR)
			{ 
			case '(':
			case '<':
				AcceptQuotationToken(CUR_CHAR);
				break;
			case '"':
			case '\'':
				AcceptStringOrChar();
				break;
			case '/':
				AcceptComment();
				break;
			default:
				AcceptDefaultToken();
			}
			ParseWhiteSpace();
		}
	}

	bool FFileParser::CheckNeedGenerateReflection()
	{
		size_t i = 0;
		for (; i < m_Tokens.size(); ++i)
		{
			const auto& CurToken = m_Tokens[i];
			if (i + 1 == m_Tokens.size())
			{
				return false;
			}
			const auto& NextToken = m_Tokens[i + 1];

			if (i + 2 == m_Tokens.size())
			{
				return false;
			}
			const auto& NextNextToken = m_Tokens[i + 2];

			std::stringstream IncludeName;
			
			IncludeName << "\"" << m_CurFilePath.filename().stem().string() << ".reflection.h\"";
			if (CurToken.TokenName == "#" && NextToken.TokenName == "include" && NextNextToken.TokenName == IncludeName.str())
			{
				i += 3;
				break;
			}
		}

		for (; i < m_Tokens.size();++i)
		{
			const auto& CurToken = m_Tokens[i];
			if (i + 1 == m_Tokens.size())
			{
				return false;
			}
			const auto& NextToken = m_Tokens[i + 1];
			if (CurToken.TokenName == "UCLASS" && NextToken.TokenName.starts_with("("))
			{
				m_ClassTagIndex = i;
				break;
			}
		}

		for (; i < m_Tokens.size();++i)
		{
			const auto& CurToken = m_Tokens[i];
			if (i + 1 == m_Tokens.size())
			{
				return false;
			}
			const auto& NextToken = m_Tokens[i + 1];
			if (CurToken.TokenName == "GENERATED_BODY" && NextToken.TokenName.starts_with("("))
			{
				m_PropertyIndex = m_ClassBodyIndex = i;
				return true;
			}
		}
		return false;
	}

	FToken FFileParser::GetType(uint32_t& TokenIndex)
	{
		int32_t LineIndex = m_Tokens[TokenIndex].LineIndex;
		std::stringstream Stream;
		std::string CurTokenName = m_Tokens[TokenIndex].TokenName;
		if (CurTokenName == "std::vector" ||  CurTokenName == "std::map")
		{
			Stream << CurTokenName;
			TokenIndex += 1;
			
			CurTokenName = m_Tokens[TokenIndex].TokenName;
			Stream << CurTokenName;
			return { Stream.str(), LineIndex };
		}

		static std::set<std::string> SimpleType = { "int", "std::string", "string", "uint32_t", "int32_t", "ZMath::vec3", "ZMath::vec4", "ZMath::FColor"};
		if (SimpleType.contains(CurTokenName))
		{
			return m_Tokens[TokenIndex];
		}
		
		if (CurTokenName.starts_with("U"))
		{
			return m_Tokens[TokenIndex];
		}

		return FToken();
	}

	void FFileParser::CollectProperty(FPropertyElement& PropertyElement)
	{
		uint32_t CurIndex = m_PropertyIndex + 2;
		bool bHasEqual = false;
		uint32_t EqualTokenIndex = -1;
		bool bHasParsedType = false;
		std::stringstream Stream;
		while (m_Tokens[CurIndex].TokenName != ";")
		{
			if (m_Tokens[CurIndex].TokenName == "const")
			{
				PropertyElement.bConst = true;
			}
			else if (m_Tokens[CurIndex].TokenName == "*")
			{
				PropertyElement.bPointer = true;
			}
			else if (m_Tokens[CurIndex].TokenName == "&")
			{
				PropertyElement.bRefercence = true;
			}
			else if (m_Tokens[CurIndex].TokenName == "static")
			{
				PropertyElement.bStatic = true;
			}
			else if (m_Tokens[CurIndex].TokenName == "=")
			{
				bHasEqual = true;
				EqualTokenIndex = CurIndex;
			}
			else if (!bHasParsedType)
			{
				bHasParsedType = true;
				FToken Token  = GetType(CurIndex);
				if (Token.IsValid())
				{
					PropertyElement.DataType = Token.TokenName;
					PropertyElement.LineIndex = Token.LineIndex;
				}
			}
			else if (!bHasEqual && bHasParsedType)
			{
				PropertyElement.Name = m_Tokens[CurIndex].TokenName;
				PropertyElement.LineIndex = m_Tokens[CurIndex].LineIndex;
			}
			else if (bHasEqual)
			{
				Stream << m_Tokens[CurIndex].TokenName;
			}
			CurIndex++;
		}
		if (bHasEqual)
		{
			PropertyElement.Value = Stream.str();
		}
		m_PropertyIndex = CurIndex;
	}

	bool FFileParser::LocatePropertyTag()
	{
		for (size_t i = m_PropertyIndex; i < m_Tokens.size() - 2; ++i)
		{
			const auto& CurToken = m_Tokens[i];
			const auto& NextToken = m_Tokens[i + 1];
			
			if (CurToken.TokenName == "UPROPERTY" && NextToken.TokenName.starts_with("("))
			{
				m_PropertyIndex = i;
				return true;;
			}
		}
		return false;
	}
	
	void FFileParser::CollectClassInfo(FClassElement& ClassElement)
	{
		ClassElement.OriginFilePath = m_CurFilePath;
		ClassElement.HeaderPath = Zero::Config::IntermediateDir / Zero::StringUtils::Format("{0}.reflection.h", m_CurFilePath.filename().stem().string());

		ClassElement.CppPath = Zero::Config::IntermediateDir / Zero::StringUtils::Format("{0}.reflection.cpp", m_CurFilePath.filename().stem().string()); 
		CLIENT_ASSERT(m_Tokens[m_ClassTagIndex + 2].TokenName == "class", "Semantic Faild!");
		ClassElement.ClassName = m_Tokens[m_ClassTagIndex + 3].TokenName;
		ClassElement.LineIndex = m_Tokens[m_ClassTagIndex + 3].LineIndex;
		size_t CurIndex = m_ClassTagIndex + 4;
		CLIENT_ASSERT(m_Tokens[CurIndex].TokenName == ":", "Semantic Faild!");
		CLIENT_ASSERT(m_Tokens[CurIndex + 1].TokenName == "public", "Semantic Faild!");
	
		ClassElement.InheritName = m_Tokens[CurIndex + 2].TokenName;

		while (LocatePropertyTag())
		{
			
			FPropertyElement PropertyElement;
			CollectProperty(PropertyElement);
			ClassElement.Properties.push_back(PropertyElement);
		}
	}

	void FFileParser::LogClassInfo(FClassElement& ClassElement)
	{
		CLIENT_LOG_INFO("Class Name : {0}", ClassElement.ClassName);
		CLIENT_LOG_INFO("Inherit : {0}", ClassElement.InheritName);
		CLIENT_LOG_INFO("Line : {0}", ClassElement.LineIndex);
		
		
		CLIENT_LOG_INFO("Propertes :");
		for (const auto& PropertyElement : ClassElement.Properties)
		{ 
			CLIENT_LOG_INFO("\tName: {0}", PropertyElement.Name);
			CLIENT_LOG_INFO("\tType: {0}", PropertyElement.DataType);
			CLIENT_LOG_INFO("\tConst: {0}", PropertyElement.bConst);
			CLIENT_LOG_INFO("\tPoint: {0}", PropertyElement.bPointer);
			CLIENT_LOG_INFO("\tStatic: {0}", PropertyElement.bStatic);
			CLIENT_LOG_INFO("\tReference: {0}", PropertyElement.bRefercence);
			CLIENT_LOG_INFO("\tValue: {0}", PropertyElement.Value);
			std::cout << "\n";
		}

		std::cout << "\n\n\n";
	}

	void FFileParser::GenerateReflectionHeaderFile(FClassElement& ClassElement)
	{

		std::vector<std::string> Contents;
		Contents.push_back("#pragma once\n\n\n");

		Contents.push_back("#ifdef REFLECTION_FILE_NAME");
		Contents.push_back("#undef REFLECTION_FILE_NAME");
		Contents.push_back("#endif //REFLECTION_FILE_NAME\n\n");

		Contents.push_back("#ifdef TAG_LINE");
		Contents.push_back("#undef TAG_LINE");
		Contents.push_back("#endif //TAG_LINE\n\n");

		Contents.push_back( Zero::StringUtils::Format("#define REFLECTION_FILE_NAME {0}", ClassElement.ClassName));
		Contents.push_back( Zero::StringUtils::Format("#define TAG_LINE {0}", ClassElement.LineIndex));

		Contents.push_back( Zero::StringUtils::Format("#define {0}_{1}_Internal_BODY \\",
			ClassElement.ClassName, 
			ClassElement.LineIndex
		));

		Contents.push_back("public: \\");
		Contents.push_back(Zero::StringUtils::Format("using Supper = {0}; \\", ClassElement.InheritName));
		Contents.push_back("static class UClass* GetClass();\\");
		Contents.push_back("protected: \\");
		Contents.push_back("\tvirtual void InitReflectionContent();");
		
		Contents.push_back( Zero::StringUtils::Format("#define {0}_{1}_GENERATED_BODY \\",
			ClassElement.ClassName, 
			ClassElement.LineIndex
		));

		Contents.push_back( Zero::StringUtils::Format("{0}_{1}_Internal_BODY",
			ClassElement.ClassName, 
			ClassElement.LineIndex
		));

		Contents.push_back(Zero::StringUtils::Format("#include \"{0}\"", ClassElement.CppPath.string()));

		std::string WholeContent = Zero::StringUtils::Join(Contents, "\n", true);
		std::cout << WholeContent;
		Zero::StringUtils::WriteFile(ClassElement.HeaderPath.string(), WholeContent);
	}

	std::string FFileParser::WriteAddPropertyCode(FClassElement& ClassElement)
	{
		//td::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize);
		std::stringstream Stream;
		for (const FPropertyElement& PropertyElement : ClassElement.Properties)
		{
			if (PropertyElement.bPointer)
			{
				Stream << "\t\tm_ClassInfoCollection.AddProperty("
					<< "\"" << PropertyElement.Name << "\", "
					<< "&(*" << PropertyElement.Name << "), "
					<< "\"" << PropertyElement.DataType << "\", "
					<< Zero::StringUtils::Format("sizeof(*{0})", PropertyElement.DataType) << ");\n";
			}
			else
			{
				Stream << "\t\tm_ClassInfoCollection.AddProperty("
					<< "\"" << PropertyElement.Name << "\", "
					<< "&" << PropertyElement.Name << ", "
					<< "\"" << PropertyElement.DataType << "\", "
					<< Zero::StringUtils::Format("sizeof({0})", PropertyElement.DataType) << ");\n";
			}
		}
		return Stream.str();
	}

	void FFileParser::GenerateReflectionCppFile(FClassElement& ClassElement)
	{
		std::vector<std::string> Contents;
		Contents.push_back("#pragma once\n\n\n");
		Contents.push_back(Zero::StringUtils::Format("#include \"{0}\"", ClassElement.OriginFilePath.string()));
		Contents.push_back("#include \"World/Base/ObjectGenerator.h\"");
		Contents.push_back("#include \"World/Base/ClassObject.h\"");
		std::stringstream Stream;
		Stream << "#ifdef _MSC_VER\n"
			<< "#pragma warning (push)\n"
			<< "#pragma warning (disable : 4883)\n"
			<< "#endif\n";
		PUSH_TO_CONTENT
		

		Stream << "namespace Zero\n"
			<< "{";
		PUSH_TO_CONTENT

		Stream << Zero::StringUtils::Format("\tUClass* {0}::GetClass()\n", ClassElement.ClassName)
			<< "\t{\n"
			<< "\t\tstatic UClass* ClassObject = nullptr;\n"
			<< "\t\tif (ClassObject != nullptr)\n"
			<< "\t\t{\n"
			<< "\t\t\tClassObject = CreateObject<UClass>(nullptr);\n"
			<< "\t\t\tClassObject->m_RegisterClassObjectDelegate.BindLambda([&]()->UCoreObject*\n"
			<< "\t\t\t{\n"
			<< "\t\t\t\treturn CreateObject<" << ClassElement.ClassName << ">(nullptr);\n"
			<< "\t\t\t});\n"
			<< "\t\t}\n"
			<< "\t\treturn ClassObject;\n"
			<< "\t}\n";
		PUSH_TO_CONTENT

		Stream << Zero::StringUtils::Format("\tvoid {0}::InitReflectionContent()\n", ClassElement.ClassName)
			<< "\t{\n"
			<< "\t\tSupper::InitReflectionContent();\n"
			<< WriteAddPropertyCode(ClassElement)
			<< "\t}\n";


		Stream << "}\n";
		PUSH_TO_CONTENT


		Stream << "#ifdef _MSC_VER\n"
			<< "#pragma warning (pop)\n"
			<< "#endif\n";
		PUSH_TO_CONTENT

		std::string WholeContent = Zero::StringUtils::Join(Contents, "\n", true);
		std::cout << WholeContent;
		Zero::StringUtils::WriteFile(ClassElement.CppPath.string(), WholeContent);
	}
}