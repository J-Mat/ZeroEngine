#include "StringDetailsMapping.h"

namespace Zero
{
	bool FStringDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
	{
		static char Buffer[256] = {0};
		std::string& Tag = *InProperty->GetData<std::string>();
		strcpy_s(Buffer, sizeof(Buffer), Tag.c_str());
		ImGui::Text(InProperty->GetEditorPropertyName());
		ImGui::SameLine();
		if (InProperty->GetClassCollection().HasField("Disable"))
		{
			ImGui::TextDisabled(Buffer);
		return false;
		}
		else if (ImGui::InputText(" ", Buffer, sizeof(Buffer)))
		{
			Tag = Buffer;		
			return true;
		}
		return false;
	}
}
