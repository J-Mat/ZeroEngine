#pragma once
#include "Component.h"
//#include "TagComponent.reflection.h" 

namespace Zero
{
	UCLASS()
	class UTagComponent : public UComponent
	{
		//GENERATED_BODY()
	public:
		UTagComponent();
		virtual ~UTagComponent();
		const std::string& GetTag() const { return m_Tag; }
		void SetTag(const std::string& Tag) { m_Tag = Tag; }
	public:
		UPROPERTY()
		std::string m_Tag;
		
	};
}