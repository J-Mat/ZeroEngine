#include "Vector4DetailsMapping.h"

namespace Zero
{
	bool FVector4DDetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		return ImGui::DragFloat4(Property->GetPropertyName(), (float*)Property->GetData());
	}
}