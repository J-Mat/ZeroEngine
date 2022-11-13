#include "CppParser.h"
#include "StringUtils.h"
#include "Log.h"
#include <string>
#include "ZConfig.h"
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
	void FFileParser::ParseWhiteSpace() {
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
			if (Steam.str() == "<<")
			{
				break;
			}
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
		m_Content = Zero::Utils::StringUtils::ReadFile(Path.string());
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

	FToken FFileParser::GetType(uint32_t& TokenIndex,bool& bIsClass)
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

		if (Zero::Utils::CodeReflectionTypes.contains(CurTokenName))
		{
			return m_Tokens[TokenIndex];
		}
		
		if (CurTokenName.starts_with("U"))
		{
			bIsClass = true;
			return m_Tokens[TokenIndex];
		}

		return FToken();
	}

	void FFileParser::CollectMetaAndField(FPropertyElement& PropertyElement)
	{
		const auto& CurToken = m_Tokens[m_PropertyIndex];
		CLIENT_ASSERT(CurToken.TokenName.starts_with("(") && CurToken.TokenName.ends_with(")"), "Semantic Faild!");
		std::string RawString = CurToken.TokenName.substr(1, CurToken.TokenName.length() - 2);
		std::vector<std::string> Features = Zero::Utils::StringUtils::Split(RawString, ",");
		for (const std::string& Feature : Features)
		{
			std::vector<std::string> KeyValue = Zero::Utils::StringUtils::Split(Feature, "=");
			if (KeyValue.size() == 1)
			{
				PropertyElement.Fields.insert(KeyValue[0]);
			}
			else
			{
				PropertyElement.Metas.insert({ KeyValue[0], KeyValue[1] });
			}
		}
	}

	void FFileParser::CollectProperty(FPropertyElement& PropertyElement)
	{
		uint32_t CurIndex = uint32_t(m_PropertyIndex + 1);
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
				FToken Token  = GetType(CurIndex, PropertyElement.bIsClass);
				if (Token.IsValid())
				{
					PropertyElement.DataType = Token.TokenName;
					PropertyElement.LineIndex = Token.LineIndex;

					if (PropertyElement.DataType.starts_with("std::vector"))
					{
						size_t StartPos = PropertyElement.DataType.find('<') + 1;
						std::string InnerValue = PropertyElement.DataType.substr(StartPos);
						InnerValue.pop_back();
						PropertyElement.InnerValue = InnerValue;
					}
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
				m_PropertyIndex = i + 1;
				return true;;
			}
		}
		return false;
	}
	
	void FFileParser::CollectClassInfo(FClassElement& ClassElement)
	{
		ClassElement.OriginFilePath = m_CurFilePath;
		ClassElement.HeaderPath = Zero::ZConfig::IntermediateDir / Zero::Utils::StringUtils::Format("{0}.reflection.h", m_CurFilePath.filename().stem().string());

		ClassElement.CppPath = Zero::ZConfig::IntermediateDir / Zero::Utils::StringUtils::Format("{0}.reflection.cpp", m_CurFilePath.filename().stem().string()); 
		CLIENT_ASSERT(m_Tokens[m_ClassTagIndex + 2].TokenName == "class", "Semantic Faild!");
		ClassElement.ClassName = m_Tokens[m_ClassTagIndex + 3].TokenName;
		ClassElement.ClassTagName = ClassElement.ClassName.substr(1);
		ClassElement.LineIndex = m_Tokens[m_ClassTagIndex + 3].LineIndex;
		size_t CurIndex = m_ClassTagIndex + 4;
		CLIENT_ASSERT(m_Tokens[CurIndex].TokenName == ":", "Semantic Faild!");
		CLIENT_ASSERT(m_Tokens[CurIndex + 1].TokenName == "public", "Semantic Faild!");
	
		std::string InhritClassName = m_Tokens[CurIndex + 2].TokenName;
		ClassElement.DerivedName = InhritClassName.starts_with("U") ? InhritClassName : "";
		
		m_DerivedRelation.insert({ClassElement.ClassName, ClassElement.DerivedName});

		while (LocatePropertyTag())
		{
			FPropertyElement PropertyElement;
			CollectMetaAndField(PropertyElement);
			CollectProperty(PropertyElement);
			ClassElement.Properties.push_back(PropertyElement);
			if (PropertyElement.InnerValue != "")
			{
				ClassElement.ArrayProperties.push_back(PropertyElement);
			}
		}
		m_AllClassElements.push_back(ClassElement);
	}

	void FFileParser::LogClassInfo(FClassElement& ClassElement)
	{
		CLIENT_LOG_INFO("Class Name : {0}", ClassElement.ClassName);
		CLIENT_LOG_INFO("Inherit : {0}", ClassElement.DerivedName);
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
		std::stringstream Stream;
		std::vector<std::string> Contents;
		Contents.push_back("#pragma once\n\n\n");

		Contents.push_back("#ifdef REFLECTION_FILE_NAME");
		Contents.push_back("#undef REFLECTION_FILE_NAME");
		Contents.push_back("#endif //REFLECTION_FILE_NAME\n\n");

		Contents.push_back("#ifdef TAG_LINE");
		Contents.push_back("#undef TAG_LINE");
		Contents.push_back("#endif //TAG_LINE\n\n");

		Contents.push_back(Zero::Utils::StringUtils::Format("#define REFLECTION_FILE_NAME {0}", ClassElement.ClassName));
		Contents.push_back(Zero::Utils::StringUtils::Format("#define TAG_LINE {0}", ClassElement.LineIndex));

		Contents.push_back(Zero::Utils::StringUtils::Format("#define {0}_{1}_Internal_BODY \\",
			ClassElement.ClassName,
			ClassElement.LineIndex
		));
		Contents.push_back("private: \\");
		Contents.push_back(Zero::Utils::StringUtils::Format("static int s_{0}ClassIndex; \\", ClassElement.ClassName));;


		Contents.push_back("public: \\");
		if (ClassElement.DerivedName != "")
		{
			Contents.push_back(Zero::Utils::StringUtils::Format("using Supper = {0}; \\", ClassElement.DerivedName));
		}
		Contents.push_back("static class UClass* GetClass();\\");
		Contents.push_back("public: \\");

		Stream << "\tvirtual const char* GetObjectName() {"
			   << std::format("return \"{0}\";", ClassElement.ClassName)
			   << "} \\";
		PUSH_TO_CONTENT



		Contents.push_back("\tvirtual void InitReflectionContent(); \\");
		Contents.push_back("\tvirtual void UpdateEditorContainerPropertyDetails(UProperty* Property);");
		
		Contents.push_back( Zero::Utils::StringUtils::Format("#define {0}_{1}_GENERATED_BODY \\",
			ClassElement.ClassName, 
			ClassElement.LineIndex
		));

		Contents.push_back( Zero::Utils::StringUtils::Format("{0}_{1}_Internal_BODY",
			ClassElement.ClassName, 
			ClassElement.LineIndex
		));

		std::string WholeContent = Zero::Utils::StringUtils::Join(Contents, "\n", true);

		std::string OriginContent = Zero::Utils::StringUtils::ReadFile(ClassElement.HeaderPath.string());
		if (OriginContent != WholeContent)
		{
			std::cout << WholeContent;
			Zero::Utils::StringUtils::WriteFile(ClassElement.HeaderPath.string(), WholeContent);
		}
	}

	bool FFileParser::IsDerived(std::string DerivedClass, std::string BasedClass)
	{
		for (auto Iter = m_DerivedRelation.find(DerivedClass); Iter != m_DerivedRelation.end(); Iter = m_DerivedRelation.find(DerivedClass))
		{
			if (BasedClass == Iter->second)
			{
				return true;
			}
			DerivedClass = Iter->second;
		}
		return false;
	}

	std::string FFileParser::WriteAddPropertyCode(const FClassElement& ClassElement)
	{
		//td::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize);
		std::stringstream Stream;
		for (const FPropertyElement& PropertyElement : ClassElement.Properties)
		{
			if (PropertyElement.bIsClass)
			{
				if (PropertyElement.bPointer)
				{
					Stream << "\t\tm_ClassInfoCollection.AddClassProperty("
						<< "\"" << ClassElement.ClassName << "\", "
						<< "\"" << PropertyElement.Name << "\", "
						<< "&(*" << PropertyElement.Name << "), "
						<< "\"" << PropertyElement.DataType << "\", "
						<< "sizeof(void*));\n";
				}
				else
				{
					Stream << "\t\tm_ClassInfoCollection.AddClassProperty("
						<< "\"" << ClassElement.ClassName << "\", "
						<< "\"" << PropertyElement.Name << "\", "
						<< "&(*" << PropertyElement.Name << "), "
						<< "\"" << PropertyElement.DataType << "\", "
						<< Zero::Utils::StringUtils::Format("sizeof(*{0})", PropertyElement.DataType) << ");\n";
				}
			}
			else
			{
				if (PropertyElement.DataType.starts_with("std::vector"))
				{
					Stream << "\t\tm_ClassInfoCollection.AddArrayProperty("
						<< "\"" << ClassElement.ClassName << "\", "
						<< "\"" << PropertyElement.Name << "\", "
						<< "&" << PropertyElement.Name << ", "
						<< "\"" << PropertyElement.DataType << "\", "
						<< Zero::Utils::StringUtils::Format("sizeof({0}), ", PropertyElement.DataType)
						<< "\"" << PropertyElement.InnerValue << "\", "
						<< Zero::Utils::StringUtils::Format("sizeof({0})", PropertyElement.InnerValue) << ");\n";
				}
				else
				{
					Stream << "\t\tm_ClassInfoCollection.AddVariableProperty("
						<< "\"" << ClassElement.ClassName << "\", "
						<< "\"" << PropertyElement.Name << "\", "
						<< "&" << PropertyElement.Name << ", "
						<< "\"" << PropertyElement.DataType << "\", "
						<< Zero::Utils::StringUtils::Format("sizeof({0})", PropertyElement.DataType) << ");\n";
				}
			}
			for (const std::string& Field : PropertyElement.Fields)
			{
				Stream << std::format("\t\tm_ClassInfoCollection.AddField(\"{0}\", \"{1}\");\n", PropertyElement.Name, Field);
			}
			for (const auto& Iter : PropertyElement.Metas)
			{
				Stream << std::format("\t\tm_ClassInfoCollection.AddMeta(\"{0}\", \"{1}\", \"{2}\");\n", PropertyElement.Name, Iter.first, Iter.second);
			}
		}
	
		return Stream.str();
	}

	std::string FFileParser::WriteUpdateContainerCode(const FClassElement& ClassElement)
	{
		std::stringstream Stream;
		Stream << "\t\tSuper::UpdateEditorContainerPropertyDetails(Property); \n";
		bool bFirst = true;
		for (const FPropertyElement& PropertyElement : ClassElement.ArrayProperties)
		{
			if (bFirst)
			{
				Stream << std::format("\t\tif Property->GetPropertyName() == \"{0}\")\n", PropertyElement.Name);
				bFirst = false;
			}
			else
			{
				Stream << std::format("\t\telse if Property->GetPropertyName() == \"{0}\")\n", PropertyElement.Name);
			}
			Stream << "\t\t{\n";
			Stream << "\t\t}\n";
		}
		return Stream.str();
	}

	void FFileParser::GenerateReflectionCppFile(const FClassElement& ClassElement)
	{
		std::vector<std::string> Contents;
		Contents.push_back(Zero::Utils::StringUtils::Format("#include \"{0}\"", ClassElement.OriginFilePath.string()));
		Contents.push_back("#include \"World/Base/ObjectGenerator.h\"");
		Contents.push_back("#include \"World/Base/ClassObject.h\"");
		Contents.push_back("#include \"World/Base/ObjectGlobal.h\"");
		std::stringstream Stream;
		Stream << "#ifdef _MSC_VER\n"
			<< "#pragma warning (push)\n"
			<< "#pragma warning (disable : 4883)\n"
			<< "#endif\n";
		PUSH_TO_CONTENT


			Stream << "namespace Zero\n"
			<< "{\n";
		PUSH_TO_CONTENT

		Stream << Zero::Utils::StringUtils::Format("\tUClass* {0}::GetClass()\n", ClassElement.ClassName)
			<< "\t{\n"
			<< "\t\tstatic UClass* ClassObject = nullptr;\n"
			<< "\t\tif (ClassObject == nullptr)\n"
			<< "\t\t{\n"
			<< "\t\t\tClassObject = CreateObjectRaw<UClass>(nullptr);\n"
			<< "\t\t\tClassObject->m_RegisterClassObjectDelegate.BindLambda([&]()->UCoreObject*\n"
			<< "\t\t\t{\n"
			<< "\t\t\t\treturn CreateObjectRaw<" << ClassElement.ClassName << ">(nullptr);\n"
			<< "\t\t\t});\n"
			<< "\t\t}\n"
			<< "\t\treturn ClassObject;\n"
			<< "\t}\n";
		PUSH_TO_CONTENT

		Stream << Zero::Utils::StringUtils::Format("\tvoid {0}::InitReflectionContent()\n", ClassElement.ClassName)
			<< "\t{\n"
			<< (ClassElement.DerivedName != "" ? "\t\tSupper::InitReflectionContent();\n" : "\n")
			<< WriteAddPropertyCode(ClassElement)
			<< "\t}\n";
		PUSH_TO_CONTENT

		Stream << Zero::Utils::StringUtils::Format("\tvoid {0}::UpdateEditorContainerPropertyDetails(UProperty* Property)\n", ClassElement.ClassName)
			<< "\t{\n"
			<< WriteUpdateContainerCode(ClassElement)
			<< "\t}\n";
		PUSH_TO_CONTENT
			

		Stream << std::format("\tint Register_{0}()\n", ClassElement.ClassName)
		<< "\t{\n"
		<< "\t\tGetClassInfoMap().insert({"
		<< Zero::Utils::StringUtils::Format("\"{1}\", FClassID(\"{0}\",{1}::GetClass()) });\n", ClassElement.DerivedName, ClassElement.ClassName)
		<< "\t\treturn 1;\n"
		<< "\t}\n"
		<< Zero::Utils::StringUtils::Format(" static int s_{0}ClassIndex = Register_{0}();", ClassElement.ClassName);
		PUSH_TO_CONTENT

		Stream << "}\n";
		PUSH_TO_CONTENT

		Stream << "#ifdef _MSC_VER\n"
			<< "#pragma warning (pop)\n"
			<< "#endif\n";
		PUSH_TO_CONTENT


		std::string WholeContent = Zero::Utils::StringUtils::Join(Contents, "\n", true);

		std::string OriginContent = Zero::Utils::StringUtils::ReadFile(ClassElement.CppPath.string());
		if (OriginContent != WholeContent)
		{
			std::cout << WholeContent;
			Zero::Utils::StringUtils::WriteFile(ClassElement.CppPath.string(), WholeContent);
		}
	}

	std::string FFileParser::WriteGenerateActors(std::vector<std::string>& ActorClassNames)
	{
		std::stringstream Stream;
		for (int i = 0; i < ActorClassNames.size(); ++i)
		{ 
			const std::string& ClassName = ActorClassNames[i];
			if (i == 0)
			{
				Stream << std::format("\t\tif (ClassName == \"{0}\")\n", ClassName);
			}
			else
			{
				Stream << std::format("\t\telse if (ClassName == \"{0}\")\n", ClassName);
			}
			Stream << "\t\t{\n";
			Stream << std::format("\t\t\treturn World->CreateActorRaw<{0}>();\n", ClassName);
			Stream << "\t\t}\n";
		}
		Stream << "\t\treturn nullptr;\n";
		return Stream.str();
	}
	
	void FFileParser::WriteLinkReflectionFile(std::set<std::filesystem::path>& AllLinkCppFiles)
	{
		std::vector<std::string> Contents;
		std::stringstream Stream;
		Contents.push_back("#pragma once\n");
	

		for (const auto& Path : AllLinkCppFiles)
		{
			Contents.push_back(Zero::Utils::StringUtils::Format("#include \"{0}\"", Path.string()));
		}
		
		std::string WholeContent = Zero::Utils::StringUtils::Join(Contents, "\n", true);
		std::string OriginFile = Zero::Utils::StringUtils::ReadFile(Zero::ZConfig::CodeReflectionLinkFile.string());

		if (OriginFile != WholeContent)
		{
			std::cout << WholeContent << std::endl;
			Zero::Utils::StringUtils::WriteFile(Zero::ZConfig::CodeReflectionLinkFile.string(), WholeContent);
			Zero::Utils::RemoveOtherFilesInDir(Zero::ZConfig::IntermediateDir.string(), AllLinkCppFiles);
		}
	}
}