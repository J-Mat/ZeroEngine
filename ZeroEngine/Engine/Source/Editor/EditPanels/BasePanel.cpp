#include "BasePanel.h"

namespace Zero
{
	void FBasePanel::UpdateWindowsInfo()
	{
		ImVec2 WindowsPos = ImGui::GetWindowPos();
		m_WindowsPos.x = int(WindowsPos.x);
		m_WindowsPos.y = int(WindowsPos.y + 39);

		m_bWindowsFocused = ImGui::IsWindowFocused();
		m_bWindowsHoverd = ImGui::IsWindowHovered();

		m_LastWindowsSize = m_WindowsSize;
		ImVec2 ImWindowsSize = ImGui::GetContentRegionAvail();
		m_WindowsSize = { ImWindowsSize.x, ImWindowsSize.y - 39 };	

		
		ImVec2 MousePos = ImGui::GetMousePos();
		m_MouseWorldPos = { MousePos.x, MousePos.y };
		m_MouseLocalPos = { MousePos.x - m_WindowsPos.x, MousePos.y - m_WindowsPos.y };
	}

	bool FBasePanel::IsMouseOutOfWindow()
	{
		if (m_MouseLocalPos.x < 0 || m_MouseLocalPos.y < 0 || m_MouseLocalPos.x > m_WindowsSize.x || m_MouseLocalPos.y > m_WindowsSize.y)
		{
			return true;
		}
		return false;
	}

	void FBasePanel::PreInitWindow()
	{
		UpdateWindowsInfo();
		UpdateMouseEvent();
	}
	
	void FBasePanel::UpdateMouseEvent()
	{
		if (!IsMouseOutOfWindow())
		{
			for (int i = 0; i < 3; ++i)
			{
				m_MouseButtonDown[i] = ImGui::IsMouseDown(i);
				m_MouseButtonClicked[i] = ImGui::IsMouseClicked(i);
			}
		}
		else
		{
			for (int i = 0; i < 3; ++i)
			{
				m_MouseButtonDown[i] = false;
				m_MouseButtonClicked[i] = false;
			}
		}
	}
}
