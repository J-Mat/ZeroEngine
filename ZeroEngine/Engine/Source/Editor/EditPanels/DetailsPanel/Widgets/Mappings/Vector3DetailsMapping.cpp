#include "Vector3DetailsMapping.h"
#include "../NumberWidget.h"

namespace Zero
{

	bool FVector3DDetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		return NumberWidget::ConstructFloat3Widget(Property);
	}
}
