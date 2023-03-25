#pragma once
#include "Core.h"

namespace Zero
{
	enum class EResourceViewType
	{
		SRV,
		RTV,
		UAV,
		DSV,
	};
	class FResourceView
	{
	public:
		FResourceView() = default;
		FResourceView(EResourceViewType Type)
		: m_ResourceViewType(Type)
		 {};
		~FResourceView() = default;
		EResourceViewType GetResourceViewType() {return m_ResourceViewType;}
		virtual void* GetNativeResource() = 0;
	protected:
		EResourceViewType m_ResourceViewType;
	};
}
