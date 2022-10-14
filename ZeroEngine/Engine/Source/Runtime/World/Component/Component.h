#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	enum class EComponentType
	{
		None = 0,
		TransformationComponent,
		TagComponent,
		MeshVertexComponent,
		MeshRenderComponent,
		CameraComponent
	};

	class UActor;
	class UComponent : public UCoreObject
	{
	public:
		UComponent()
			: UCoreObject()
		{
		}
		void SetParentComponent(UComponent* Component) { m_Parent = Component; };
	protected:
		UComponent* m_Parent = nullptr;
	};
}