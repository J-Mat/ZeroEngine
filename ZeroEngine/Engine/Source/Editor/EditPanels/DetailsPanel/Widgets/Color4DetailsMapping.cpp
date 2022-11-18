#include "Color4DetailsMapping.h"

namespace Zero
{
	void FColor4DetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		m_bEdited = ImGui::ColorEdit3(ProperyTag, (float*)Property->GetData<ZMath::FColor4>());
	}
}
