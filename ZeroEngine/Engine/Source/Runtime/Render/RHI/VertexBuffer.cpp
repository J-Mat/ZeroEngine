#include "VertexBuffer.h"

namespace Zero
{
	FVertexBufferLayout FVertexBufferLayout::s_TestVertexLayout
	{
		FBufferElement::s_Pos,
		FBufferElement::s_Color
	};

	FVertexBufferLayout FVertexBufferLayout::s_DefaultVertexLayout
	{
		FBufferElement::s_Pos,
		FBufferElement::s_Color
	};
}