#pragma once
#include "Component.h"
#include "TagComponent.reflection.h"

namespace Zero
{
	UClass()
	class UTagComponent : public UComponent
	{
		GENERATED_UCLASS_BODY()
	public:
		COMPONENT_CLASS_TYPE(TagComponent)
		UTagComponent(const std::string Tag);
		virtual ~UTagComponent();
		const std::string& GetTag() const { return m_Tag; }
		void SetTag(const std::string& Tag) { m_Tag = Tag; }
	public:
		UPROPERTY()
		std::string m_Tag;
	};
}