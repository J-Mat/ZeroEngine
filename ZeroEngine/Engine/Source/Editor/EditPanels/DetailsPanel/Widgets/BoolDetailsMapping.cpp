#include "BoolDetailsMapping.h"

namespace Zero
{
	bool FBoolDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
	{
		FVariableDetailsMapping::UpdateDetailsWidget(InProperty);
		return ImGui::Checkbox(InProperty->GetEditorPropertyName(), InProperty->GetData<bool>());
	}
}
