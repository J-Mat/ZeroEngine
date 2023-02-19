#include "FloatDetailsMapping.h"

namespace Zero
{
	void FFloatDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		m_bEdited = ImGui::InputFloat(ProperyTag, Property->GetData<float>());
	}
}
