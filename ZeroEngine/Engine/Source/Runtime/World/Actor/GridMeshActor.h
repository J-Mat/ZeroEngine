#pragma once

#include "Core.h"
#include "MeshActor.h"
#include "GridMeshActor.reflection.h"

namespace Zero
{
	UCLASS()
	class UGridMeshActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		UGridMeshActor();
		UGridMeshActor(float Width, float Height, uint32_t Rows, uint32_t Cols);
		virtual void BuildMesh();
	private:
		float m_Width = 20.0f;
		float m_Height = 30.0f;
		uint32_t  m_Rows = 60;
		uint32_t  m_Cols = 40;
	};
}