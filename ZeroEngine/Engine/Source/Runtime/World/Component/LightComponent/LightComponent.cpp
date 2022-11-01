#include "LightComponent.h"

namespace Zero
{
	void ULightComponent::PostInit()
	{
		m_TransformCompent = static_cast<UTransformComponent*>(m_Parent);
	}
}
