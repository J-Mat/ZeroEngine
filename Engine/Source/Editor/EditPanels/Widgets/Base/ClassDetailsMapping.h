#pragma once
#include "DetailsMapping.h"
#include "World/Object/PropertyObject.h"

namespace Zero
{
	class FClassDetailsMapping : public FDetailsMapping
	{
	public:
		void UpdateDetailsWidget(UCoreObject* CoreObject)
		{
			UpdateDetailsWidgetImpl(CoreObject);
		}
		static Ref<FClassDetailsMapping> MakeDetailMapping();
	private:
		virtual void UpdateDetailsWidgetImpl(UCoreObject* CoreObject) = 0;
	};
}