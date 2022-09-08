#include "MeshVertexComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"
#include "CubeMeshVertexComponent.h"

namespace Zero
{ 
	UCubeMeshVertexComponent::UCubeMeshVertexComponent()
		:UMeshVertexComponent()
	{
		
		std::vector<float> Vertices = 
		{
			-1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,
			-1.0f, +1.0f, -1.0f,   DirectX::Colors::Black[0],   DirectX::Colors::Black[1],  DirectX::Colors::Black[2],
			+1.0f, +1.0f, -1.0f,   DirectX::Colors::Red[0,],
			+1.0f, -1.0f, -1.0f,   DirectX::Colors::Green) }),
			-1.0f, -1.0f, +1.0f,   DirectX::Colors::Blue) }),
			-1.0f, +1.0f, +1.0f,   DirectX::Colors::Yellow) }),
			+1.0f, +1.0f, +1.0f,   DirectX::Colors::Cyan) }),
			+1.0f, -1.0f, +1.0f,   DirectX::Colors::Magenta) })
		}
		std::vector<uint32_t> Indices =
		{
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};
	}
}
