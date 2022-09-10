#include "MeshVertexComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"

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
				PerObtConstantsDesc.Mapper.InsertConstant(Element, ERootParameters::PerObjCB);
			}
		}

		return PerObtConstantsDesc;
	}

	Ref<IShaderConstantsBuffer> FPerObjectConstantsBufferPool::GetPerObjectConstantsBuffer(UCoreObject* Obj)
	{
		Ref<IShaderConstantsBuffer> Result;
		if (m_IdleConstantsBuffer.empty())
		{

			auto Device = Obj->GetWorld()->GetDevice();
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


	UMeshVertexComponent::UMeshVertexComponent(FMeshType& MeshType)
		: UComponent()
		, m_ShaderConstantsBuffer(FPerObjectConstantsBufferPool::GetInstance().GetPerObjectConstantsBuffer(this))
		, m_MeshType(MeshType)
	{
		m_Mesh = CreateRef<FMesh>();
		FMeshCreator::GetInstance().CreatMesh(m_MeshType, m_MeshData);
		FVertexBufferLayout VertexBufferLayout;
		VertexBufferLayout = { FBufferElement::s_Pos, FBufferElement::s_Color};
		m_Mesh = FRenderer::GraphicFactroy->CreateMesh(
			GetWorld()->GetDevice().get(),
			m_MeshData.m_Vertices.data(),
			m_MeshData.m_VertexNum,
			m_MeshData.m_Indices.data(),
			m_MeshData.m_IndexNum,
			VertexBufferLayout
		);
	}

	UMeshVertexComponent::~UMeshVertexComponent()
	{
		FPerObjectConstantsBufferPool::GetInstance().PushToPool(m_ShaderConstantsBuffer);
	}
}
