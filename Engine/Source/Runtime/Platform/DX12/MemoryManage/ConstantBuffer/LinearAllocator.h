#pragma once

#include "Core.h"
#include "../../Common/DX12Header.h"

namespace Zero
{
	inline constexpr size_t const INVALID_OFFSET = static_cast<size_t>(-1);
	class FLinearAllocator
	{
	public:

		FLinearAllocator(size_t Maxsize, size_t Reserve = 0) noexcept :
			m_MaxSize{ Maxsize }, m_Reserve{ Reserve }, m_Top{ m_Reserve }
		{}
		FLinearAllocator(FLinearAllocator const&) = default;
		FLinearAllocator& operator = (FLinearAllocator const&) = delete;
		FLinearAllocator(FLinearAllocator&&) = default;
		FLinearAllocator& operator=(FLinearAllocator&&) = delete;
		~FLinearAllocator() = default;

		size_t Allocate(size_t Size, size_t Align = 0)
		{
			auto AlignedTop = ZMath::AlignUp(m_Top, Align);
			if (AlignedTop + Size > m_MaxSize)
			{
				return INVALID_OFFSET;
			}
			else
			{
				size_t Start = AlignedTop;
				AlignedTop += Size;
				m_Top = AlignedTop;
				return Start;
			}
		}

		void Clear()
		{
			m_Top = m_Reserve;
		}

		size_t MaxSize()  const { return m_MaxSize; }
		bool Full()			  const { return m_Top == m_MaxSize; };
		bool Empty()		  const { return m_Top == m_Reserve; };
		size_t UsedSize() const { return m_Top; }

	private:
		size_t const m_MaxSize;
		size_t const m_Reserve;
		size_t m_Top;
	};
}