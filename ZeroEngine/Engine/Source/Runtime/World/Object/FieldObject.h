#pragma once
#include "CoreObject.h"

namespace Zero
{
	class  UField : public UCoreObject
	{
		using Supper = UCoreObject;
	public: 
		UField();

		UField* Next = nullptr;
	};
}