#include "IntDetailsMapping.h"

namespace Zero
{
	void Zero::FIntDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		m_bEdited = ImGui::InputInt(ProperyTag, Property->GetData<int32_t>());
	}
}
