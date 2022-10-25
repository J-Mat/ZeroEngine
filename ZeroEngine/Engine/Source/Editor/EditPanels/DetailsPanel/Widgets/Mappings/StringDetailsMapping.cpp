#include "StringDetailsMapping.h"

namespace Zero
{
	bool FStringDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
	{
		char Buffer[256];
		memset(Buffer, 0, sizeof(Buffer));
		std::string& Tag = *InProperty->GetData<std::string>();
		strcpy_s(Buffer, sizeof(Buffer), Tag.c_str());
		if (ImGui::InputText(InProperty->GetName().c_str(), Buffer, sizeof(Buffer)))
		{
			Tag = Buffer;		
		}
		return false;
	}
}
