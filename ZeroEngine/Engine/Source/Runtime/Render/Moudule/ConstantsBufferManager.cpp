#include "ConstantsBufferManager.h"
#include "Render/RHI/ShaderData.h"
#include "Render/RHI/GraphicDevice.h"
#include "Render/RendererAPI.h"


namespace Zero
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static FShaderConstantsDesc GlobalConstantsDesc;
	static FShaderConstantsDesc& GetGlobalConstantsDesc()
	{
		if (GlobalConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_GlobalConstants;
			int paraIndex = 0;

			GlobalConstantsDesc.Size = Layout.GetStride();
			for (auto Element : Layout)
			{
				GlobalConstantsDesc.Mapper.InsertConstant(Element);
			}
		}

		return GlobalConstantsDesc;
	}

	FConstantsBufferManager::FConstantsBufferManager()
		: m_GlobalConstantsBuffer(FRenderer::GraphicFactroy->CreateShaderConstantBuffer(GetGlobalConstantsDesc()))
	{
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static FShaderConstantsDesc PerCameraConstantsDesc;
	static FShaderConstantsDesc& GetPerCameraConstantsDesc()
	{
		// Init if not Inited
		if (PerCameraConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_PerCameraConstants;
			int paraIndex = 0;

			PerCameraConstantsDesc.Size = Layout.GetStride();
			for (auto BufferElement : Layout)
			{
				PerCameraConstantsDesc.Mapper.InsertConstant(BufferElement);
			}
		}

		return PerCameraConstantsDesc;
	}
	Ref<IShaderConstantsBuffer> FConstantsBufferManager::GetCameraConstantBuffer()
	{
		Ref<IShaderConstantsBuffer> Result;
		if (m_IdleCameraConstantsBuffer.empty())
		{
			Result = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(GetPerCameraConstantsDesc());
		}
		else
		{
			Result = m_IdleCameraConstantsBuffer.front();
			m_IdleCameraConstantsBuffer.pop();
		}

		return Result;
	}

	Ref<IShaderConstantsBuffer> FConstantsBufferManager::GetMaterialConstBufferByDesc(Ref<FShaderConstantsDesc> Desc)
	{
		return FRenderer::GraphicFactroy->CreateShaderConstantBuffer(*Desc.get());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static FShaderConstantsDesc PerObjectConstantsDesc;
	static FShaderConstantsDesc& GetPerObjConstantsDesc()
	{
		if (PerObjectConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_PerObjectConstants;
			int paraIndex = 0;

			PerObjectConstantsDesc.Size = Layout.GetStride();
			for (auto Element : Layout)
			{
				PerObjectConstantsDesc.Mapper.InsertConstant(Element);
			}
		}

		return PerObjectConstantsDesc;
	}
	Ref<IShaderConstantsBuffer> FConstantsBufferManager::GetPerObjectConstantsBuffer()
	{
		Ref<IShaderConstantsBuffer> Result;
		if (m_IdlePerObjectConstantsBuffer.empty())
		{
			Result = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(GetPerObjConstantsDesc());
		}
		else
		{
			Result = m_IdlePerObjectConstantsBuffer.front();
			m_IdlePerObjectConstantsBuffer.pop();
		}

		return Result;
	}

	void FConstantsBufferManager::PushPerObjectConstantsBufferToPool(Ref<IShaderConstantsBuffer> Buffer)
	{
		m_IdlePerObjectConstantsBuffer.push(Buffer);
	}
	void FConstantsBufferManager::PushCameraConstantsBufferToPool(Ref<IShaderConstantsBuffer> Buffer)
	{
		m_IdleCameraConstantsBuffer.push(Buffer);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
