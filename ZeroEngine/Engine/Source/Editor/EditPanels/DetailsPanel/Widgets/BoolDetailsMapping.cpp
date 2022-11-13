#include "BoolDetailsMapping.h"

namespace Zero
{
	void FBoolDetailsMapping::UpdateDetailsWidgetImpl(UProperty* InProperty)
	{
		 m_bEdited = ImGui::Checkbox(InProperty->GetEditorPropertyName(), InProperty->GetData<bool>());
	}
}
