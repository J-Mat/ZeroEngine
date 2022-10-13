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

#define COMPONENT_CLASS_TYPE(Type) static  EComponentType GetStaticType() {return EComponentType::##Type;}\
								virtual EComponentType GetEventType() const override {return GetStaticType();}
	class UActor;
	class UComponent : public UCoreObject
	{
	public:
		UComponent()
			: UCoreObject()
		{
		}
		virtual EComponentType GetEventType() const = 0;
		void SetParentComponent(UComponent* Component) { m_Parent = Component; };
	protected:
		UComponent* m_Parent = nullptr;
	};
}