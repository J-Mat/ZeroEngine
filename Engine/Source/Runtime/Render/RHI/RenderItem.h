#pragma once

#include "Core.h"
#include "Render/RHI/Mesh.h"
#include "Render/RenderConfig.h"
#include "Render/Moudule/Material.h"

namespace Zero
{
	class IShaderConstantsBuffer;
	class FGraphicPipelineStateObject;
	class FComputePipelineStateObject;

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
		
		Ref<FGraphicPipelineStateObject> GetPsoObj(const FRenderParams& RenderSettings);
		Ref<FGraphicPipelineStateObject> GetPsoObj();
	private:
		Ref<FGraphicPipelineStateObject> m_PipelineStateObject = nullptr;
	};


	class FComputeRenderItem
	{
	public:
		FComputeRenderItem();
		~FComputeRenderItem() {};

		void Reset();
		Ref<FComputePipelineStateObject> GetPsoObj();
		void Compute(FCommandListHandle ComamndListHandle, uint32_t NumGroupsX, uint32_t NumGroupsY, uint32_t NumGroupsZ);
		void SetPsoID(uint32_t PsoID) { m_PsoID = PsoID; }
		Ref<FShaderParamsGroup> GetShaderParamsGroup() { return  m_ShaderParamsGroup;}
	private:
		uint32_t m_PsoID = ZERO_INVALID_ID;
		Ref<FComputePipelineStateObject> m_PipelineStateObject = nullptr;
		Ref<FShaderParamsGroup> m_ShaderParamsGroup = nullptr;
	};


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


	class FComputeRenderItemPool
	{
	public:
		FComputeRenderItemPool() = default;
		void Reset();
		Ref<FComputeRenderItem> Request();
		void Push(Ref<FComputeRenderItem> Item);
		using iter = std::vector<Ref<FComputeRenderItem>>::iterator;
		iter begin() { return m_ComputeRenderItems.begin(); }
		iter end() { return m_ComputeRenderItems.end(); }
	public:
		std::vector<Ref<FComputeRenderItem>> m_ComputeRenderItems;
		std::vector<Ref<FComputeRenderItem>> m_AvailableRenderItems;
	};
}
