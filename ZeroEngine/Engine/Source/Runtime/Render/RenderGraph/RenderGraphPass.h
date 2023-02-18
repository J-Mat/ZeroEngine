#pragma once
#include "Core.h"
#include "Utils/EnumUtil.h"
#include "RenderGraphContext.h"
#include "Render/RenderConfig.h"

namespace Zero
{	
	enum class ERGPassFlags : uint32_t
	{
		None = 0x00,
		ForceNoCull = 0x01,						//RGPass cannot be culled by Render Graph
		AllowUAVWrites = 0x02,					//Allow uav writes, only makes sense if LegacyRenderPassEnabled is disabled
		SkipAutoRenderPass = 0x04,				//RGPass will manage render targets by himself
		LegacyRenderPassEnabled = 0x08,			//Don't use DX12 Render Passes, use OMSetRenderTargets
		ActAsCreatorWhenWriting = 0x10			//When writing to a resource, avoid forcing dependency by acting as a creator
	};
	DEFINE_ENUM_BIT_OPERATORS(ERGPassFlags);

	enum ERGReadAccess : uint8_t
	{
		ReadAccess_PixelShader,
		ReadAccess_NonPixelShader,
		ReadAccess_AllShader
	};

	enum class ERGLoadAccessOp : uint8_t
	{
		Discard,
		Preserve,
		Clear,
		NoAccess
	};
	
	enum class ERGStoreAccessOp : uint8_t
	{
		Discard,
		Preserve,
		Resolve,
		NoAccess
	};

	inline constexpr uint8_t CombineAccessOps(ERGLoadAccessOp LoadOp, ERGStoreAccessOp StoreOp)
	{
		return (uint8_t)LoadOp << 2 | (uint8_t)StoreOp;
	}

	enum class ERGLoadStoreAccessOp : uint8_t
	{
		Discard_Discard = CombineAccessOps(ERGLoadAccessOp::Discard, ERGStoreAccessOp::Discard),
		Discard_Preserve = CombineAccessOps(ERGLoadAccessOp::Discard, ERGStoreAccessOp::Preserve),
		Clear_Preserve = CombineAccessOps(ERGLoadAccessOp::Clear, ERGStoreAccessOp::Preserve),
		Preserve_Preserve = CombineAccessOps(ERGLoadAccessOp::Preserve, ERGStoreAccessOp::Preserve),
		Clear_Discard = CombineAccessOps(ERGLoadAccessOp::Clear, ERGStoreAccessOp::Discard),
		Preserve_Discard = CombineAccessOps(ERGLoadAccessOp::Preserve, ERGStoreAccessOp::Discard),
		Clear_Resolve = CombineAccessOps(ERGLoadAccessOp::Clear, ERGStoreAccessOp::Resolve),
		Preserve_Resolve = CombineAccessOps(ERGLoadAccessOp::Preserve, ERGStoreAccessOp::Resolve),
		Discard_Resolve = CombineAccessOps(ERGLoadAccessOp::Discard, ERGStoreAccessOp::Resolve),
		NoAccess_NoAccess = CombineAccessOps(ERGLoadAccessOp::NoAccess, ERGStoreAccessOp::NoAccess),
	};

	inline constexpr void SplitAccessOp(ERGLoadStoreAccessOp LoadstoreOp, ERGLoadAccessOp& load_op, ERGStoreAccessOp& StoreOp)
	{
		StoreOp = static_cast<ERGStoreAccessOp>((uint8_t)LoadstoreOp & 0b11);
		load_op = static_cast<ERGLoadAccessOp>(((uint8_t)LoadstoreOp >> 2) & 0b11);
	}
	
	class FRenderGraph;
	class FRenderGraphBuilder;

	class FRGPassBase
	{
		friend FRenderGraph;
		friend FRenderGraphBuilder;
	
		struct FRenderTargetInfo
		{
			FRGRenderTargetID RGRenderTargetID;
			ERGLoadStoreAccessOp RenderTargetAccess;
		};
		struct FDepthStencilInfo
		{
			FRGDepthStencilID RGDepthStencilID;
			ERGLoadStoreAccessOp DepthAccess;
			ERGLoadStoreAccessOp StencilAccess;
			bool bReadOnly;
		};
	public:
		FRGPassBase(const char * Name, ERenderPassType Type = ERenderPassType::Graphics, ERGPassFlags Flags = ERGPassFlags::None)
			:  m_Name(Name), m_Type(Type), m_Flags(Flags)
		{}
		virtual ~FRGPassBase() = default;
		ERenderPassType GetPassType() const { return m_Type; }
	protected:
		virtual void Setup(FRenderGraphBuilder&) = 0;
		virtual void Execute(FRenderGraphContext&, FCommandListHandle CommandListHandle) const = 0;

		bool IsCulled() const { return CanBeCulled() && m_RefCount == 0; }
		bool CanBeCulled() const { return !HasAnyFlag(m_Flags, ERGPassFlags::ForceNoCull); }
		bool SkipAutoRenderPassSetup() const { return HasAnyFlag(m_Flags, ERGPassFlags::SkipAutoRenderPass); }
		bool UseLegacyRenderPasses() const { return HasAnyFlag(m_Flags, ERGPassFlags::LegacyRenderPassEnabled); }
		bool AllowUAVWrites() const { return HasAnyFlag(m_Flags, ERGPassFlags::AllowUAVWrites); }
		bool ActAsCreatorWhenWriting() const { return HasAnyFlag(m_Flags, ERGPassFlags::ActAsCreatorWhenWriting); };

	private:
		std::string m_Name;
		ERenderPassType m_Type;
		ERGPassFlags m_Flags;
		size_t m_RefCount = 0ull;
		
		std::set<FRGTextureID> m_TextureCreates;
		std::set<FRGTextureID> m_TextureReads;
		std::set<FRGTextureID> m_TextureWrites;
		std::set<FRGTextureID> m_TextureDestroys;
		std::map<FRGTextureID, EResourceState> m_TextureStateMap;

		std::set<FRGBufferID> m_BufferCreates;
		std::set<FRGBufferID> m_BufferReads;
		std::set<FRGBufferID> m_BufferWrites;
		std::set<FRGBufferID> m_BufferDestroys;
		std::map<FRGBufferID, EResourceState> m_BufferStateMap;
		
		std::vector<FRenderTargetInfo> m_RenderTergetInfo;
		std::optional<FDepthStencilInfo> m_DepthStencil = std::nullopt;
		uint32_t m_VieportWidth = 0;
		uint32_t m_VieportHeight = 0;
	};
	
	template<typename FPassData>
	class FRederGraphPass final : public FRGPassBase
	{
	public:
		using SetupFunc = std::function<void(FPassData&, FRenderGraphBuilder&)> ;
		using ExcuteFunc = std::function<void(FPassData&, FRenderGraphContext&, FCommandListHandle)>;
		FRederGraphPass(const char* Name, SetupFunc&& Setup, ExcuteFunc&& Execute, ERenderPassType Type = ERenderPassType::Graphics, ERGPassFlags Flags = ERGPassFlags::None)
			: FRGPassBase(Name, Type, Flags),
			m_SetupFunc(std::move(Setup)),
			m_ExcuteFunc(std::move(Execute))
		{}
		
		FPassData const& GetPassData() const
		{
			return m_PassData;
		}
	private:
		FPassData m_PassData;
		SetupFunc m_SetupFunc;
		ExcuteFunc m_ExcuteFunc;
	private:
		void Setup(RenderGraphBuilder& Builder) override
		{
			ADRIA_ASSERT(m_SetupFunc != nullptr, "setup function is null!");
			m_SetupFunc(m_PassData, Builder);
		}

		void Execute(FRenderGraphContext& Context, FCommandListHandle CommandListHandle) const override
		{
			CORE_ASSERT(m_ExcuteFunc != nullptr, "execute function is null!");
			m_ExcuteFunc(m_PassData, Context, CommandListHandle);
		}
	};
	template<>
	class FRederGraphPass<void> final : public FRGPassBase
	{
	public:
		using SetupFunc = std::function<void(FRenderGraphBuilder&)> ;
		using ExcuteFunc = std::function<void(FRenderGraphContext&, FCommandListHandle)>;
		FRederGraphPass(char const* Name, SetupFunc&& Setup, ExcuteFunc&& Execute, ERenderPassType Type = ERenderPassType::Graphics, ERGPassFlags Flags = ERGPassFlags::None)
			: FRGPassBase(Name, Type, Flags),
			m_SetupFunc(std::move(Setup)),
			m_ExcuteFunc(std::move(Execute))
		{}
	private:
		SetupFunc m_SetupFunc;
		ExcuteFunc m_ExcuteFunc;
	private:
		void Setup(FRenderGraphBuilder& Builder) override
		{
			m_SetupFunc(Builder);
		}

		void Execute(FRenderGraphContext& Context, FCommandListHandle CommandListHandle) const override
		{
			CORE_ASSERT(m_ExcuteFunc != nullptr, "execute function is null!");
			m_ExcuteFunc(Context, CommandListHandle);
		}
	};
}