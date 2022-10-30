#include "Vector4DetailsMapping.h"

namespace Zero
{
	bool FVector4DDetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		return ImGui::DragFloat4(Property->GetEditorPropertyName(), (float*)Property->GetData());
	}
}