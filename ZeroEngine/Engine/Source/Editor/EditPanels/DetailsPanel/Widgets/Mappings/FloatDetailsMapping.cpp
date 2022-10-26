#include "FloatDetailsMapping.h"

namespace Zero
{
	bool FFloatDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
	{
		return ImGui::DragFloat(InProperty->GetPropertyName(), InProperty->GetData<float>());
	}
}
