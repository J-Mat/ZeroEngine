#include "FloatDetailsMapping.h"

namespace Zero
{
	bool FFloatDetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		FVariableDetailsMapping::UpdateDetailsWidget(Property);
		return ImGui::DragFloat(" ", Property->GetData<float>());
	}
}
