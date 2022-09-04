#include "MeshVertexComponent.h"
#include "Render/RendererAPI.h"

namespace Zero
{ 
	static FShaderConstantsDesc PerObtConstantsDesc;
	static FShaderConstantsDesc& GetPerObjectConstantsDesc()
	{
		if (PerObtConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_PerObjectConstants;
			int paraIndex = 0;

			PerObtConstantsDesc.Size = Layout.GetStride();
			for (auto Element : Layout)
			{
				PerObtConstantsDesc.Mapper.InsertConstant(Element, 1);
			}
		}

		return PerObtConstantsDesc;
	}

	Ref<IShaderConstantsBuffer> FPerObjectConstantsBufferPool::GetPerObjectConstantsBuffer(uint32_t DeviceIndex)
	{
		Ref<IShaderConstantsBuffer> Result;
		if (m_IdleConstantsBuffer.empty())
		{

			auto Device = FRenderer::GetDevice(DeviceIndex);
			Result = FRenderer::GraphicFactroy->CreateShaderConstantBuffer(Device.get(), GetPerObjectConstantsDesc());
		}
		else
		{
			Result = m_IdleConstantsBuffer.front();
			m_IdleConstantsBuffer.pop();
		}

		return Result;
	}

	void FPerObjectConstantsBufferPool::PushToPool(Ref<IShaderConstantsBuffer> Buffer)
	{
		m_IdleConstantsBuffer.push(Buffer);
	}


	UMeshVertexComponent::UMeshVertexComponent(uint32_t DeviceIndex)
		: UComponent(DeviceIndex)
		, m_ShaderConstantsBuffer(FPerObjectConstantsBufferPool::GetInstance().GetPerObjectConstantsBuffer(m_DeviceIndex))
	{
	}

	UMeshVertexComponent::~UMeshVertexComponent()
	{
		FPerObjectConstantsBufferPool::GetInstance().PushToPool(m_ShaderConstantsBuffer);
	}
}
