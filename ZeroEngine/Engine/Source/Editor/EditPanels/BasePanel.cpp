#include "BasePanel.h"

namespace Zero
{
	void FBasePanel::UpdateWindowsInfo()
	{
		ImVec2 WindowsPos = ImGui::GetWindowPos();
		m_WindowsPos.x = int(WindowsPos.x);
		m_WindowsPos.y = int(WindowsPos.y);

		m_bWindowsFocused = ImGui::IsWindowFocused();
		m_bWindowsHoverd = ImGui::IsWindowHovered();

		m_LastWindowsSize = m_WindowsSize;
		ImVec2 ImWindowsSize = ImGui::GetContentRegionAvail();
		m_WindowsSize = { ImWindowsSize.x, ImWindowsSize.y };	
	}

	bool FBasePanel::IsMouseOutOfWindow()
	{
		if (m_MousePos.x < 0 || m_MousePos.y < 0 || m_MousePos.x > m_WindowsSize.x || m_MousePos.y > m_WindowsSize.y)
		{
			return true;
		}
		return false;
	}
}
