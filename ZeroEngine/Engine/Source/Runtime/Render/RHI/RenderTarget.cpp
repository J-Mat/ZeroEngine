#include "RenderTarget.h"
#include "Texture.h"

namespace Zero
{
	FRenderTarget::FRenderTarget()
	{
	}
	void FRenderTarget::Resize(uint32_t Width, uint32_t Height)
	{
		for (int i = 0; i < EAttachmentIndex::NumAttachmentPoints; ++i)
		{
			m_Textures[i]->Resize(Width, Height);
		}
	}
}
