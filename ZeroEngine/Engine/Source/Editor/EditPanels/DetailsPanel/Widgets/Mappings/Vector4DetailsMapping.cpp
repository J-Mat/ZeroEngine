#include "Vector4DetailsMapping.h"
#include "../NumberWidget.h"

namespace Zero
{
	bool FVector4DDetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		return NumberWidget::ConstructFloat4Widget(Property);
	}
}