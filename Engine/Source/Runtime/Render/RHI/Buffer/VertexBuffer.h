#pragma once
#include "Core.h"
#include "Buffer.h"
#include "../ShaderData.h"
#include "../GraphicDevice.h"


namespace Zero
{
	class FVertexBufferLayout
	{
	public:
		FVertexBufferLayout() {}
		FVertexBufferLayout(const std::initializer_list<FBufferElement>& _ElementList)
			:ElementsList(_ElementList)
		{
			CalculateOffsetsAndStride();
		}
		static FVertexBufferLayout StandardVertexBufferLayout;

		bool HasProperty(const std::string& ProppertyName) const
		{
			for (const auto& Element : ElementsList)
			{
				if (Element.Name == ProppertyName)
				{
					return true;
				}
			}
			return false;
		}
		bool operator==(const FVertexBufferLayout& Other) const
		{
			return ElementsList == Other.ElementsList;
		}
		bool operator!=(const FVertexBufferLayout& Other) const
		{
			return !(*this == Other);
		}

		inline const std::vector<FBufferElement>& GetElementsList() const { return ElementsList; }
		inline const uint32_t GetStride() const { return Stride; }
		std::vector<FBufferElement>::iterator begin() { return ElementsList.begin(); }
		std::vector<FBufferElement>::iterator end() { return ElementsList.end(); }

		void CalculateOffsetsAndStride()
		{
			uint32_t Offset = 0;
			for (auto& Element : ElementsList)
			{
				Element.Offset = Offset;
				Offset += Element.Size; 
				Stride += Element.Size;
			}
		}
	static FVertexBufferLayout s_DefaultVertexLayout;
	static FVertexBufferLayout s_SkyboxVertexLayout;
	static FVertexBufferLayout s_DebugVertexLayout;

	private:
		std::vector<FBufferElement> ElementsList;
		uint32_t Stride = 0;
	};


	class FVertexBuffer : public FBuffer
	{
	public:
		FVertexBuffer(void* Data, const FBufferDesc& Desc, FVertexBufferLayout& _Layout)
			: FBuffer(Data, Desc),
			m_Layout(_Layout)
		{}
	public:
		FVertexBufferLayout m_Layout;
	};
}