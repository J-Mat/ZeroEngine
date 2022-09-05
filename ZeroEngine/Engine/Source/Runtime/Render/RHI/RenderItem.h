#pragma once
#include "Core.h"

namespace Zero
{
	struct FSubMesh;
	class FMesh;
	class FMaterial;
	class IShaderConstantsBuffer;
	class FRenderItem;
	
	class FRenderItemPool
	{
	public:
		FRenderItemPool() = default;
		void Reset();
		Ref<FRenderItem> Request();
		void Push(Ref<FRenderItem> Item);
		using iter = std::vector<Ref<FRenderItem>>::iterator;
		iter begin() { return m_RenderItems.begin(); }
		iter end() { return m_RenderItems.end(); }
	private:
		std::vector<Ref<FRenderItem>> m_RenderItems;
		std::vector<Ref<FRenderItem>> m_AvailableRenderItems;
	};

	class FRenderItem
	{
	public:
		FRenderItem() = default;
		FRenderItem(Ref<FMesh> Mesh);
		FRenderItem(Ref<FMesh> Mesh, Ref<FSubMesh> SubMesh);
		~FRenderItem();
		void SetModelMatrix(const ZMath::mat4& Transform);
		void OnDrawCall();
		Ref<FMaterial> m_Material = nullptr;
		Ref<FMesh> m_Mesh = nullptr;
		Ref<FSubMesh> m_SubMesh = nullptr;
		Ref<IShaderConstantsBuffer> m_ConstantsBuffer = nullptr;

	};
}
