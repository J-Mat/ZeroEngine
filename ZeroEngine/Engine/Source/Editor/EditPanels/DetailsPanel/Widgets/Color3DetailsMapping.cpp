#include "Color3DetailsMapping.h"

namespace Zero
{
	void FColor3DetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property)
	{
		m_bEdited = ImGui::ColorEdit3(Property->GetEditorPropertyName(), (float*)Property->GetData<ZMath::FColor3>());
	}
}
