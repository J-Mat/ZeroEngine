#include "Vector4DetailsMapping.h"

namespace Zero
{
	void FVector4DDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		m_bEdited = ImGui::DragFloat4(Property->GetEditorPropertyName(), (float*)Property->GetData());
	}
}