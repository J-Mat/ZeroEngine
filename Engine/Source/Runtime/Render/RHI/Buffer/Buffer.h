#pragma once
#include "Core.h"
#include "Render/ResourceCommon.h"

namespace Zero
{
	struct FBufferDesc
	{
		uint32_t Size = 0;
		EResourceUsage ResourceUsage = EResourceUsage::Default;
		EResourceBindFlag ResourceBindFlag = EResourceBindFlag::None;
		EBufferMiscFlag BufferMiscFlag = EBufferMiscFlag::None;
		uint32_t Stride = 0;
		uint32_t Count = 0;
		EResourceFormat Format = EResourceFormat::UNKNOWN;
		std::strong_ordering operator<=>(FBufferDesc const& Other) const = default;

		static FBufferDesc VertexBufferDesc(uint32_t VertexCount, uint32_t Stride)
		{
			FBufferDesc Desc
			{
				.Size = VertexCount * Stride,
				.ResourceUsage = EResourceUsage::Default,
				.ResourceBindFlag = EResourceBindFlag::None,
				.Stride = Stride,
				.Count = VertexCount
			};
			return Desc;
		}

		static FBufferDesc IndexBufferDesc(uint32_t IndexCount, bool bSmallIndex = false)
		{
			FBufferDesc Desc
			{
				.Size = IndexCount * (bSmallIndex ? 2 : 4),
				.ResourceUsage = EResourceUsage::Default,
				.ResourceBindFlag = EResourceBindFlag::None,
				.Stride = bSmallIndex ? uint32_t(2) : uint32_t(4),
				.Count = IndexCount,	
				.Format = bSmallIndex ? EResourceFormat::R16_UINT : EResourceFormat::R32_UINT
			};
			return Desc;
		}
	};


	struct FBufferSubresourceDesc
	{
		uint64_t Offset;
		uint64_t Size;
		std::strong_ordering operator<=>(FBufferSubresourceDesc const& Other) const = default;
	};

	class FBuffer
	{
	public:
		FBuffer(const FBufferDesc& Desc, void* Data) :
			m_Desc(Desc),
			m_Data(Data)
		{}

		FBuffer(std::string BufferName, const FBufferDesc& Desc) :
			m_BufferName(BufferName),
			m_Desc(Desc)
		{}
		const FBufferDesc& GetDesc() { return m_Desc; }
	protected:
		FBufferDesc m_Desc;
		std::string m_BufferName;
		void* m_Data = 0;
	};
}