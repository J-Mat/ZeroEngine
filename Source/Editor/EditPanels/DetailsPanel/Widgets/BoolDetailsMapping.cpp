#include "BoolDetailsMapping.h"

namespace Zero
{
	void FBoolDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		 m_bEdited = ImGui::Checkbox(ProperyTag, Property->GetData<bool>());
	}
}
