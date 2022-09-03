#pragma once
#include "Core.h"

namespace Zero
{
	struct FSubMesh;
	class FMesh;
	class FMaterial;
	class IShaderConstantsBuffer;
	class FDrawItem
	{
	public:
		FDrawItem() = default;
		FDrawItem(Ref<FMesh> Mesh);
		FDrawItem(Ref<FMesh> Mesh, Ref<FSubMesh> SubMesh);
		~FDrawItem();
		void SetModelMatrix(const ZMath::mat4& Transform);
		void OnDrawCall();
	private:
		Ref<FMaterial> m_Material = nullptr;
		Ref<FMesh> m_Mesh = nullptr;
		Ref<FSubMesh> m_SubMesh = nullptr;
		Ref<IShaderConstantsBuffer> m_ConstantsBuffer = nullptr;

	};
}
