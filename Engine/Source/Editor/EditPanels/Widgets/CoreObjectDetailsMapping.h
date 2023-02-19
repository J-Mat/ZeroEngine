#pragma once

#include "Base/ClassDetailsMapping.h"

namespace Zero
{
	class FCoreObjectDetailsMapping : public FClassDetailsMapping
	{
	public:
		static Ref<FClassDetailsMapping> MakeDetailMapping() { return CreateRef<FCoreObjectDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UCoreObject* CoreObject) override;
	};
}