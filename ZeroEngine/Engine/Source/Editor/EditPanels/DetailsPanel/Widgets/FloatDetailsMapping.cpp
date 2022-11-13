#include "FloatDetailsMapping.h"

namespace Zero
{
	void FFloatDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property)
	{
		m_bEdited = ImGui::DragFloat(" ", Property->GetData<float>());
	}
}
