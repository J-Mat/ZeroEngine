#pragma once

#include <ZeroEngine.h>


namespace Zero
{
	class FBasePanel
	{
	public:
		FBasePanel() = default;
		virtual void Init() {}
		virtual void OnGuiRender() = 0;
		void UpdateWindowsInfo();
		bool IsMouseOutOfWindow();
		ZMath::ivec2 GetWindowsPos() { return m_WindowsPos;}
		ZMath::ivec2 GetWindowsSize() { return m_WindowsSize;}
		virtual void OnMouseClick(int X, int Y) {}
	protected:
		ZMath::ivec2 m_WindowsPos;
		ZMath::ivec2 m_MousePos;
		ZMath::ivec2  m_WindowsSize;
		ZMath::ivec2 m_LastWindowsSize;
		bool m_bWindowsFocused = false;
		bool m_bWindowsHoverd = false;
	};
}	