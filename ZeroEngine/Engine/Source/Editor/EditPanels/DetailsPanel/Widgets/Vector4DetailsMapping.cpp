#include "Vector4DetailsMapping.h"

namespace Zero
{
	void FVector4DDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property)
	{
		m_bEdited = ImGui::DragFloat4(Property->GetEditorPropertyName(), (float*)Property->GetData());
	}
}