#pragma once

#include "Base/VariableDetailsMapping.h"
#include "Asset/AssetObject/MaterialAsset.h"

namespace Zero
{
	class FMaterialHandleDetailsMapping : public FVariableDetailsMapping
	{
	public:
		virtual bool UpdateDetailsWidget(UProperty* InProperty);
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FMaterialHandleDetailsMapping>(); }
	private:
		UMaterialAsset* m_MaterialAsset = nullptr;
	};
}