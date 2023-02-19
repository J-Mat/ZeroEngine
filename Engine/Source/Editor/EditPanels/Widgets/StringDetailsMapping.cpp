#include "StringDetailsMapping.h"

namespace Zero
{
	void FStringDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		static char Buffer[256] = {0};
		std::string& Tag = *Property->GetData<std::string>();
		strcpy_s(Buffer, sizeof(Buffer), Tag.c_str());
		if (Property->GetClassCollection().HasField("UnEditabe"))
		{
			ImGui::TextDisabled(Buffer);
			m_bEdited = false;
		}
		else if (ImGui::InputText(ProperyTag, Buffer, sizeof(Buffer)))
		{
			Tag = Buffer;		
			m_bEdited = true;
		}
	}
}
