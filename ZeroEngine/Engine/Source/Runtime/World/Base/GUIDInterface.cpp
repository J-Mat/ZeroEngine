#include "GUIDInterface.h"

Zero::IGUIDInterface::IGUIDInterface()
	:m_GUID(FGUID::GenerateGUID())
{
}
