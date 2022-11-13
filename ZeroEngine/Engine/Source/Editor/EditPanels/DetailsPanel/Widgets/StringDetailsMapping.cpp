#include "StringDetailsMapping.h"

namespace Zero
{
	void FStringDetailsMapping::UpdateDetailsWidgetImpl(UProperty* InProperty)
	{
		static char Buffer[256] = {0};
		std::string& Tag = *InProperty->GetData<std::string>();
		strcpy_s(Buffer, sizeof(Buffer), Tag.c_str());
		if (InProperty->GetClassCollection().HasField("Disable"))
		{
			ImGui::TextDisabled(Buffer);
			m_bEdited = false;
		}
		else if (ImGui::InputText(" ", Buffer, sizeof(Buffer)))
		{
			Tag = Buffer;		
			m_bEdited = true;
		}
	}
}
