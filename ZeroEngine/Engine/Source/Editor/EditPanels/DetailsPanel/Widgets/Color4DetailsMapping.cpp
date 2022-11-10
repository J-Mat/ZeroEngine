#include "Color4DetailsMapping.h"

namespace Zero
{
	bool FColor4DetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		FVariableDetailsMapping::UpdateDetailsWidget(Property);
		return ImGui::ColorEdit3(" ", (float*)Property->GetData<ZMath::FColor4>());
	}
}
