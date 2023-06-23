#pragma once
#include <GUI.h>

class FDetailsMapping : public std::enable_shared_from_this<FDetailsMapping>
{
protected:
	bool m_bEdited = false;
};