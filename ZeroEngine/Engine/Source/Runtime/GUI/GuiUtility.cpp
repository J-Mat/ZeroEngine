#include "GuiUtility.h"
#include "Render/RendererAPI.h"

namespace ImGui
{
	void DrawImage(ImTextureID user_texture_id, const ImVec2& size)
	{
		switch (Zero::FRenderer::RHI)
		{
		case Zero::ERHI::DirectX12:
			ImGui::Image(user_texture_id, size,
				{ 0,1 }, { 1, 0 });
			break;
		case Zero::ERHI::OpenGL:
			ImGui::Image(user_texture_id, size,
				{ 0,0 }, { 1, 1 });
			break;
		default:
			break;
		}
	}
}