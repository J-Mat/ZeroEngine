#include "TagComponent.h"

namespace Zero
{
	UTagComponent::UTagComponent(const std::string Tag)
		:UComponent()
		,m_Tag(Tag)
	{
		m_bTick = false;
	}

	UTagComponent::~UTagComponent()
	{
	}
}
