#include "CppParser.h"
#include "StringUtils.h"
#include "Log.h"
#include <string>


#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISLETTER(ch)        (((ch) >= 'a' && (ch) <= 'z') ||  ((ch) >= 'A' && (ch) <= 'Z'))
#define ISLETTER_OR_DIGIT(ch)  (ISDIGIT(ch) || ISLETTER(ch))
#define CUR_CHAR m_Content[m_CurPos] 
#define PRE_CHAR m_Content[m_CurPos - 1] 
#define INCREASE_INDEX m_CurPos++
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
			if (ISLETTER_OR_DIGIT(CUR_CHAR))
			{ 
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
		std::cout << m_Content;
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
		for (auto& Token : m_Tokens)
		{
			std::cout << Token.TokenName << " "  << Token.LineIndex << std::endl;
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
			IncludeName << "\"" << m_CurFilePath.filename().stem() << ".reflection.h\"";
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
			if (CurToken.TokenName == "UCLASS" && CurToken.TokenName.starts_with("("))
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
			if (CurToken.TokenName == "REFLECTION_BODY" && CurToken.TokenName.starts_with("("))
			{
				m_PropertyIndex = m_ClassBodyIndex = i;
				return true;
			}
		}
		return false;
	}

	static std::string GetType(uint32_t TokenIndex)
	{

	}

	void FFileParser::CollectProperty(FPropertyElement& PropertyElement)
	{
		uint32_t CurIndex = m_PropertyIndex + 2;
		bool bHasEqual = false;
		uint32_t EqualTokenIndex = -1;
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
			CurIndex++;
		}

		if (bHasEqual)
		{
			PropertyElement.Name = m_Tokens[EqualTokenIndex - 1].TokenName;
			PropertyElement.DataType =  
		}
	}

	bool FFileParser::LocatePropertyTag(FClassElement& ClassElement)
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
		ClassElement.Path = m_CurFilePath;

		CLIENT_ASSERT(m_Tokens[m_ClassTagIndex + 1].TokenName == "class", "Semantic Faild!");
		ClassElement.ClassName = m_Tokens[m_ClassBodyIndex + 2].TokenName;
		ClassElement.LineIndex = m_Tokens[m_ClassBodyIndex + 2].LineIndex;
		size_t CurIndex = m_ClassBodyIndex + 3;
		CLIENT_ASSERT(m_Tokens[CurIndex].TokenName == ":", "Semantic Faild!");
		CLIENT_ASSERT(m_Tokens[CurIndex + 1].TokenName == "public", "Semantic Faild!");
	
		ClassElement.InheritName = m_Tokens[CurIndex + 2].TokenName;

		while (LocatePropertyTag())
		{
			CollectProperty();
		}
		
	}

	void FFileParser::GenerateCodeReflectionFile()
	{
	}
}