#pragma once

#include "Core.h"
#include "Actor.h"
#include "Render/Moudule/MeshGenerator.h"
#include "Render/RHI/ShaderData.h"
#include "MeshActor.reflection.h"

namespace Zero
{ 
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class FRenderItemPool;
	UCLASS()
	class UMeshActor : public UActor
	{
		GENERATED_BODY()
	public:
		UMeshActor();
		virtual void BuildMesh() {};
		virtual void PostInit() override;
		virtual void CommitToPipieline();
		virtual void Tick();
		virtual ZMath::FAABB GetAABB() override;
		
		void SetParameter(const std::string& ParameterName, EShaderDataType ShaderDataType, void* ValuePtr);

	protected:
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		UMeshRenderComponent* m_MeshRenderComponent = nullptr;
	};
}