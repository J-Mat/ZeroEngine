#include "VertexBuffer.h"

namespace Zero
{
	static FVertexBufferLayout s_DefaultVertexLayout
	{
		FBufferElement::s_Pos,
		FBufferElement::s_Normal,
		FBufferElement::s_Tangent,
		FBufferElement::s_Tex
	};
}