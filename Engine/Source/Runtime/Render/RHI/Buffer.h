#pragma once
#include "Core.h"
#include "Render/ResourceCommon.h"
#include "Shader/ShaderData.h"

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
		const FBufferDesc& GetDesc() const { return m_Desc; }
		FBufferDesc& GetDesc() { return m_Desc; }
		uint32_t GetCount() const { return static_cast<UINT>(m_Desc.Size / m_Desc.Stride);}
		virtual void* Map() { return nullptr; }
		virtual void Unmap() {}
		virtual void Update(void const* SrcData, size_t DataSize, size_t Offset = 0) {};
		virtual void* GetNative() { return nullptr; }
	protected:
		FBufferDesc m_Desc;
		std::string m_BufferName;
		void* m_Data = 0;
	};
}