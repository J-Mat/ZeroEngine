#include "Color4DetailsMapping.h"

namespace Zero
{
	void FColor4DetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property)
	{
		m_bEdited = ImGui::ColorEdit3(" ", (float*)Property->GetData<ZMath::FColor4>());
	}
}
