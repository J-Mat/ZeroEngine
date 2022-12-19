#pragma once
#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/Shader.h"

namespace Zero
{
	class FConstantsBufferManager : public IPublicSingleton<FConstantsBufferManager>
	{
	public:
		FConstantsBufferManager();
		virtual ~FConstantsBufferManager() = default;
		Ref<IShaderConstantsBuffer> GetGlobalConstantBuffer() { return m_GlobalConstantsBuffer; }
		Ref<IShaderConstantsBuffer> GetCameraConstantBuffer();
		Ref<IShaderConstantsBuffer> GetMaterialConstBufferByDesc(Ref<FShaderConstantsDesc> Desc);

		Ref<IShaderConstantsBuffer> GetPerObjectConstantsBuffer();
		void PushPerObjectConstantsBufferToPool(Ref<IShaderConstantsBuffer> Buffer);
	protected:
		Ref<IShaderConstantsBuffer> m_GlobalConstantsBuffer = nullptr;

		std::queue<Ref<IShaderConstantsBuffer>> m_IdlePerObjectConstantsBuffer;
	};
}