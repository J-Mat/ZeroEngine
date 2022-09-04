#pragma once
#include "Component.h"
namespace Zero
{
	class UTagComponent : public UComponent
	{
	public:
		UTagComponent(const std::string Tag);
		virtual ~UTagComponent();
		const std::string& GetTag() const { return m_Tag; }
		void SetTag(const std::string& Tag) { m_Tag = Tag; }
	private:
		std::string m_Tag;
	};
}