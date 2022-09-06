#include "GUID.h"

namespace Zero
{
    uint32_t FGUID::HashIndex = 0;
	FGUID::FGUID(GUID Guid)
		: m_GUID(Guid)
        , m_HashValue(FGUID::HashIndex++)
	{
	}
	FGUID FGUID::GenerateGUID()
    {
        GUID ID;
        CORE_ASSERT(CoCreateGuid(&ID) == S_OK, "Genrerate UUID failed !");
        return FGUID(ID);
    }

    std::string FGUID::ToString() const
    {
        char Buffer[GUID_LEN] = { 0 };
		snprintf(Buffer, sizeof(Buffer),
			"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
            m_GUID.Data1, m_GUID.Data2, m_GUID.Data3,
            m_GUID.Data4[0], m_GUID.Data4[1], m_GUID.Data4[2],
            m_GUID.Data4[3], m_GUID.Data4[4], m_GUID.Data4[5],
            m_GUID.Data4[6], m_GUID.Data4[7]);
        
        return std::string(Buffer);
    }

}
