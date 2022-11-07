#pragma once

#include "Base/ClassDetailsMapping.h"

namespace Zero
{
	class FAssetDetailsMapping : public FClassDetailsMapping
	{
	public:
		virtual void UpdateDetailsWidget(UCoreObject* CoreObject);
		static Ref<FClassDetailsMapping> MakeDetailMapping() { return CreateRef<FAssetDetailsMapping>(); }
	};
}