#pragma once
#include <ZeroEngine.h>

class FDetailsMapping : public std::enable_shared_from_this<FDetailsMapping>
{
protected:
	bool m_bEdited = false;
};