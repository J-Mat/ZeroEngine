#pragma once
#include "Core.h"
#include "Render/RHI/Mesh.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class FMaterial;
	class IShaderConstantsBuffer;
	class FRenderItem;
	class FPipelineStateObject;
	

	class FRenderItemPool
	{
	public:
		FRenderItemPool() = default;
		void Reset();
		Ref<FRenderItem> Request();
		void Push(Ref<FRenderItem> Item);
		void AddGuid(const Utils::Guid& Guid) { m_RenderGuids.push_back(Guid); }
		std::vector<Utils::Guid>& GetRenderGuids() { return m_RenderGuids; }
		using iter = std::vector<Ref<FRenderItem>>::iterator;
		iter begin() { return m_RenderItems.begin(); }
		iter end() { return m_RenderItems.end(); }
	public:
		std::vector<Ref<FRenderItem>> m_RenderItems;
		std::vector<Ref<FRenderItem>> m_AvailableRenderItems;
		std::vector<Utils::Guid> m_RenderGuids;
	public:
		static Ref<FRenderItemPool> s_DIYRenderItemPool;
	};

	class FRenderItem
	{
	public:
		FRenderItem() = default;
		FRenderItem(Ref<FMesh> Mesh);
		FRenderItem(Ref<FMesh> Mesh, const FSubMesh& SubMesh);
		~FRenderItem();
		void SetModelMatrix(const ZMath::mat4& Transform);
		Ref<FMaterial> m_Material = nullptr;
		uint32_t m_PsoID = ZERO_INVALID_ID;
		Ref<FMesh> m_Mesh = nullptr;
		FSubMesh m_SubMesh;
		Ref<IShaderConstantsBuffer> m_PerObjectBuffer = nullptr;
		ZMath::mat4 m_Transform;
		bool CanRender(FCommandListHandle ComamndListHandle, const FRenderParams& RenderSettings);
		void Render(FCommandListHandle ComamndListHandle);
		void Reset();
		
		Ref<FPipelineStateObject> GetPsoObj(const FRenderParams& RenderSettings);
		Ref<FPipelineStateObject> GetPsoObj();
	private:
		Ref<FPipelineStateObject> m_PipelineStateObject = nullptr;
	};
}
