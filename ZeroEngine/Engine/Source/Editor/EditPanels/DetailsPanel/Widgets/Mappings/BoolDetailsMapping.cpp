#include "BoolDetailsMapping.h"

namespace Zero
{
	bool FBoolDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
	{
		return ImGui::Checkbox(InProperty->GetName().c_str(), InProperty->GetData<bool>());
	}
}
