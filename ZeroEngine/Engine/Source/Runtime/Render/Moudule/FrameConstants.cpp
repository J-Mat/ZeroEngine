#include "FrameConstants.h"
#include "Render/RHI/ShaderData.h"
#include "Render/RHI/GraphicDevice.h"
#include "Render/RendererAPI.h"


namespace Zero
{
	static FShaderConstantsDesc PerObtConstantsDesc;
	static FShaderConstantsDesc& GetPerObjectConstantsDesc()
	{
		if (PerObtConstantsDesc.Size == -1)
		{
			FConstantBufferLayout& Layout = FConstantBufferLayout::s_PerFrameConstants;
			int paraIndex = 0;

			PerObtConstantsDesc.Size = Layout.GetStride();
			for (auto Element : Layout)
			{
				PerObtConstantsDesc.Mapper.InsertConstant(Element, ERootParameters::FrameConstantCB);
			}
		}

		return PerObtConstantsDesc;
	}
	FFrameConstants::FFrameConstants(IDevice* Device)
		: m_ShaderConstantsBuffer(FRenderer::GraphicFactroy->CreateShaderConstantBuffer(Device, GetPerObjectConstantsDesc()))
	{
	}
}
