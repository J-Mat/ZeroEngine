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
		virtual void OnMouseClick() {}
		bool MouseButtonDown(ImGuiMouseButton Button) { return m_MouseButtonDown[Button];}
		bool MouseButtonReleased(ImGuiMouseButton Button) { return m_MouseButtonReleased[Button];}
	protected:
		void UpdateMouseEvent();
		void PreInitWindow();
		ZMath::ivec2 m_WindowsPos;
		ZMath::ivec2 m_MouseLocalPos;
		ZMath::ivec2 m_MouseWorldPos;
		ZMath::ivec2 m_WindowsSize;
		ZMath::ivec2 m_LastWindowsSize;
		bool m_bWindowsFocused = false;
		bool m_bWindowsHoverd = false;
		bool m_MouseButtonDown[3] = {false, false, false};
		bool m_MouseButtonReleased[3] = {false, false, false};
	};
}	