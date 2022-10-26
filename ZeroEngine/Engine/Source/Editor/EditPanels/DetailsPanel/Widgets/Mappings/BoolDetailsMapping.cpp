#include "BoolDetailsMapping.h"

namespace Zero
{
	bool FBoolDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
	{
		return ImGui::Checkbox(InProperty->GetPropertyName(), InProperty->GetData<bool>());
	}
}
