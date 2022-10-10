#pragma once
#include "Header.h"


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



	class FCppParser
	{
	public:
		FCppParser() = default;
		void Parse(std::filesystem::path Path);
		n t ye ojg y n
	};

}
