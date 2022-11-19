#pragma once

#include "Base/ClassDetailsMapping.h"

namespace Zero
{
	class FActorDetailsMapping : public FClassDetailsMapping
	{
	public:
		static Ref<FClassDetailsMapping> MakeDetailMapping() { return CreateRef<FActorDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UCoreObject* CoreObject) override;
		void ShowComponentObject(UComponent* Component);
	};
}