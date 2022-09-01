#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "GraphicDevice.h"


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
	private:
		std::vector<FBufferElement> ElementsList;
		uint32_t Stride = 0;
	};

	class IVertexBuffer
	{
	public:
		enum class EType
		{
			Static,
			Dynamic,
			Stream,
		};
		IVertexBuffer(void* _Data, uint32_t _VertexCount, FVertexBufferLayout& _Layout, IVertexBuffer::EType _Type)
			: m_Data(_Data),
			m_VertexCount(_VertexCount),
			m_Layout(_Layout),
			m_Type(_Type),
			m_BufferSize(_VertexCount * m_Layout.GetStride())
		{}
	protected:
		void* m_Data = nullptr;
		uint32_t m_VertexCount = 0;
		FVertexBufferLayout m_Layout;
		EType m_Type;
		size_t m_BufferSize = 0;
	};
}