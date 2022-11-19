#pragma once

#include "Base/VariableDetailsMapping.h"
#include "Asset/AssetObject/MaterialAsset.h"

namespace Zero
{
	class FMaterialHandleDetailsMapping : public FVariableDetailsMapping
	{
	public:
		static Ref<FVariableDetailsMapping> MakeDetailMapping() { return CreateRef<FMaterialHandleDetailsMapping>(); }
	private: 
		virtual void UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag) override;
		void CheckMaterialAsset(FMaterialHandle* MaterialHandle);
	private:
		UMaterialAsset* m_MaterialAsset = nullptr;
	};
}