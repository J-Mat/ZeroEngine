#include "ClassObject.h"

namespace Zero
{
	UClass::UClass()
	{
	}
	UCoreObject* UClass::GetDefaultObject()
	{
		if (m_DefaultObject == nullptr)
		{
			if (m_RegisterClassObjectDelegate.IsBound())
			{
				m_DefaultObject = m_RegisterClassObjectDelegate.Execute();
				if (m_DefaultObject != nullptr)
				{
					m_DefaultObject->SetOuter(this);
				}
			}
		}
		
		return m_DefaultObject;
	}
}
