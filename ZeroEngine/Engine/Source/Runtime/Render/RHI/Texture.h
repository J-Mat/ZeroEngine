#pragma once
#include "Core.h"
#include "Render/ResourceCommon.h"

namespace Zero
{
	struct FTextureClearValue
	{
		enum class EActiveMember
		{
			None,
			Color,
			DepthStencil
		};
		struct ClearColor
		{
			ClearColor(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f)
				: Color{ r, g, b, a }
			{
			}
			ClearColor(float(&_color)[4])
				: Color{ _color[0], _color[1], _color[2], _color[3] }
			{
			}
			ClearColor(ClearColor const& other)
				: Color{ other.Color[0], other.Color[1], other.Color[2], other.Color[3] }
			{
			}

			bool operator==(ClearColor const& other) const
			{
				return memcmp(Color, other.Color, sizeof(Color)) == 0;
			}

			float Color[4];
		};
		struct ClearDepthStencil
		{
			ClearDepthStencil(float Depth = 0.0f, uint8_t Stencil = 1)
				: Depth(Depth), Stencil(Stencil)
			{}
			float Depth;
			uint8_t Stencil;
		};

		FTextureClearValue() : EActiveMember(EActiveMember::None), DepthStencil{} {}

		FTextureClearValue(float r, float g, float b, float a)
			: EActiveMember(EActiveMember::Color), Color(r, g, b, a)
		{
		}

		FTextureClearValue(float(&_color)[4])
			: EActiveMember(EActiveMember::Color), Color{ _color }
		{
		}

		FTextureClearValue(ClearColor const& Color)
			: EActiveMember(EActiveMember::Color), Color(Color)
		{}

		FTextureClearValue(float Depth, uint8_t Stencil)
			: EActiveMember(EActiveMember::DepthStencil), DepthStencil(Depth, Stencil)
		{}
		FTextureClearValue(ClearDepthStencil const& DepthStencil)
			: EActiveMember(EActiveMember::DepthStencil), DepthStencil(DepthStencil)
		{}

		FTextureClearValue(FTextureClearValue const& other)
			: EActiveMember(other.EActiveMember), Color{}
		{
			if (EActiveMember == EActiveMember::Color) Color = other.Color;
			else if (EActiveMember == EActiveMember::DepthStencil) DepthStencil = other.DepthStencil;
		}

		bool operator==(FTextureClearValue const& other) const
		{
			if (EActiveMember != other.EActiveMember) return false;
			else if (EActiveMember == EActiveMember::Color)
			{
				return Color == other.Color;
			}
			else return DepthStencil.Depth == other.DepthStencil.Depth
				&& DepthStencil.Stencil == other.DepthStencil.Stencil;
		}

		EActiveMember EActiveMember;
		union
		{
			ClearColor Color;
			ClearDepthStencil DepthStencil;
		};
	};


	enum class ETextureType : uint8_t
	{
		TextureType_1D,
		TextureType_2D,
		TextureType_3D
	};

	using FFrameBufferTexturesFormats = std::vector<EResourceFormat>;

	struct FTextureDesc
	{
		ETextureType Type = ETextureType::TextureType_2D;
		uint64_t Width = 0;
		uint32_t Height = 0;
		uint32_t Depth = 0;
		uint32_t ArraySize = 1;
		uint16_t MipLevels = 1;
		uint32_t SampleCount = 1;
		EResourceUsage HeapType = EResourceUsage::Default;
		EResourceBindFlag ResourceBindFlags = EResourceBindFlag::None;
		EResourceState InitialState = EResourceState::PixelShaderResource | EResourceState::NonPixelShaderResource;
		FTextureClearValue ClearValue{};
		EResourceFormat Format = EResourceFormat::UNKNOWN;
		
		std::strong_ordering operator<=>(FTextureDesc const& Other) const = default;
		bool IsCompatible(FTextureDesc const& Desc) const
		{
			return Type == Desc.Type && Width == Desc.Width && Height == Desc.Height && ArraySize == Desc.ArraySize
				&& Format == Desc.Format && SampleCount == Desc.SampleCount && HeapType == Desc.HeapType
				&& HasAllFlags(ResourceBindFlags, Desc.ResourceBindFlags);
		}
	};

	struct FTextureSubresourceDesc
	{
		uint32_t FirstSlice;
		uint32_t SliceCount;
		uint32_t FirstMip;
		uint32_t MipCount;
		std::strong_ordering operator<=>(FTextureSubresourceDesc const& Other) const = default;
	};

	class ITexture
	{
	public:
		ITexture() = default;
		ITexture(const FTextureDesc& TextureDesc) :
			m_TextureDesc(TextureDesc)
		{}
		virtual ~ITexture() = default;
		virtual uint64_t GetWidth() { return m_TextureDesc.Width; };
		virtual uint32_t GetHeight() { return m_TextureDesc.Height; };
		virtual void Bind(uint32_t Slot) {};
		FTextureDesc const& GetDesc() const { return m_TextureDesc; }
	protected:
		FTextureDesc m_TextureDesc;
	}; 

	class FImage;
	class FTexture2D : public ITexture
	{
	public:
		FTexture2D(bool bNeetMipMap = false) :
			m_bNeedMipMap(bNeetMipMap) 
		{}
		FTexture2D(const FTextureDesc &TextureDesc ) :
			ITexture(TextureDesc)
		{}
		virtual ~FTexture2D() = default;
		virtual Ref<FImage> GetImage() { return m_ImageData; };
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1) = 0;
		virtual ZMath::uvec2 GetSize() = 0;
		virtual void RegistGuiShaderResource() = 0;
		virtual UINT64 GetGuiShaderReseource() = 0;
		virtual void MakeSRVs(const std::vector<FTextureSubresourceDesc>& Descs) {};
		virtual void MakeRTVs(const std::vector<FTextureSubresourceDesc>& Descs) {};
		virtual void MakeDSVs(const std::vector<FTextureSubresourceDesc>& Descs) {};
		virtual void MakeUAVs(const std::vector<FTextureSubresourceDesc>& Descs) {};
	protected:	
        Ref<FImage> m_ImageData = nullptr;
		bool m_bNeedMipMap = false;
	};

	class FTextureCubemap : public ITexture
	{
	public:
		enum class Format
		{
			R8G8B8,
			R8G8B8A8,
			R32G32B32A32,
			R24G8,
		};
		virtual ~FTextureCubemap() = default;
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1) = 0;
		virtual ZMath::uvec2 GetSize() = 0;
	};
}