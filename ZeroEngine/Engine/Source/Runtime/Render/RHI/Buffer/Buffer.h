#pragma once
#include "Core.h"
#include "Render/ResourceCommon.h"

namespace Zero
{
	struct FBufferDesc
	{
		uint64_t Size = 0;
		EResourceUsage ResourceUsage = EResourceUsage::Default;
		EResourceBindFlag ResourceBindFlag = EResourceBindFlag::None;
		EBufferMiscFlag BufferMiscFlag = EBufferMiscFlag::None;
		uint32_t Stride = 0;
		uint32_t Count = 0;
		EResourceFormat Format = EResourceFormat::UNKNOWN;
		std::strong_ordering operator<=>(FBufferDesc const& other) const = default;

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
				.Stride = bSmallIndex ? 2 : 4,
				.Count = IndexCount,	
				.Format = bSmallIndex ? EResourceFormat::R16G16_UINT : EResourceFormat::R32G32_UINT
			};
			return Desc;
		}
	};

	class FBuffer
	{
	public:
		FBuffer(void* Data, const FBufferDesc& Desc) :
			m_Data(Data),
			m_Desc(Desc)
		{}
		const FBufferDesc& GetDesc() { return m_Desc; }
	protected:
		FBufferDesc m_Desc;
		void* m_Data = 0;
	};
}