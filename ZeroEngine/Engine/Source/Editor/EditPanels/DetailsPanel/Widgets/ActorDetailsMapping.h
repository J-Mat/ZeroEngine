#pragma once

#include "Base/ClassDetailsMapping.h"

namespace Zero
{
	class FActorDetailsMapping : public FClassDetailsMapping
	{
	public:
		virtual void UpdateDetailsWidget(UCoreObject* CoreObject);
		static Ref<FClassDetailsMapping> MakeDetailMapping() { return CreateRef<FActorDetailsMapping>(); }
	private:
		void ShowComponentObject(UComponent* Component, int ID);
	};
}