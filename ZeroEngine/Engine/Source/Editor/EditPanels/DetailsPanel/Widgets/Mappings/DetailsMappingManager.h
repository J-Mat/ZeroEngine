#pragma once
#include "Core/Base/PublicSingleton.h"
#include "Base/ClassDetailsMapping.h"
#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FDetailMappingManager : public IPublicSingleton<FDetailMappingManager>
	{
	public:
		void RegisterVariableMapping(EPropertyType PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping);
		Ref<FVariableDetailsMapping> FindPropertyMapping(EPropertyType PrpertyType);
		
		bool UpdatePropertyWidgets(UVariableProperty* Property);
		bool UpdateClassWidgets(UCoreObject* CoreObject);

	private:
		std::map<EPropertyType, Ref<FVariableDetailsMapping>> m_ProppertyMapping;
	};
	
}