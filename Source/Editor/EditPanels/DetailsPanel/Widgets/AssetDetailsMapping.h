#pragma once

#include "Base/ClassDetailsMapping.h"

namespace Zero
{
	class FAssetDetailsMapping : public FClassDetailsMapping
	{
	public:
		static Ref<FClassDetailsMapping> MakeDetailMapping() { return CreateRef<FAssetDetailsMapping>(); }
	private:
		virtual void UpdateDetailsWidgetImpl(UCoreObject* CoreObject) override;
	};
}