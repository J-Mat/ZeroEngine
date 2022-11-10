#include "Color3DetailsMapping.h"

namespace Zero
{
	bool FColor3DetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		FVariableDetailsMapping::UpdateDetailsWidget(Property);
		return ImGui::ColorEdit3(Property->GetEditorPropertyName(), (float*)Property->GetData<ZMath::FColor3>());
	}
}
