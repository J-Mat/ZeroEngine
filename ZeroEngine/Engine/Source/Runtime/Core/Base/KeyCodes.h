#pragma once
#include "Core.h"
#include "Render/RendererAPI.h"

inline int GetKeyCode(int GLFWCode, int WinCode)
{
	switch (Zero::FRenderer::GetRHI())
	{
	case Zero::ERHI::DirectX12: return WinCode;
	case Zero::ERHI::OpenGL:    return GLFWCode; 
	default: return -1; break;
	}
}

/* The unknown key */
#define SIByL_KEY_UNKNOWN            (GetKeyCode(-1,-1))

/* Printable keys */
#define SIByL_KEY_SPACE              (GetKeyCode(32 ,VK_SPACE))
#define SIByL_KEY_APOSTROPHE         (GetKeyCode(39	,VK_OEM_7))			/* ' */
#define SIByL_KEY_COMMA              (GetKeyCode(44	,VK_OEM_COMMA))		/* , */
#define SIByL_KEY_MINUS              (GetKeyCode(45	,VK_OEM_MINUS))		/* - */
#define SIByL_KEY_PERIOD             (GetKeyCode(46	,VK_OEM_2))			/* . */
#define SIByL_KEY_SLASH              (GetKeyCode(47	,0x30))				/* / */
#define SIByL_KEY_0                  (GetKeyCode(48	,0x30))
#define SIByL_KEY_1                  (GetKeyCode(49	,0x31))
#define SIByL_KEY_2                  (GetKeyCode(50	,0x32))
#define SIByL_KEY_3                  (GetKeyCode(51	,0x33))
#define SIByL_KEY_4                  (GetKeyCode(52	,0x34))
#define SIByL_KEY_5                  (GetKeyCode(53	,0x35))
#define SIByL_KEY_6                  (GetKeyCode(54	,0x36))
#define SIByL_KEY_7                  (GetKeyCode(55	,0x37))
#define SIByL_KEY_8                  (GetKeyCode(56	,0x38))
#define SIByL_KEY_9                  (GetKeyCode(57	,0x39))
#define SIByL_KEY_SEMICOLON          (GetKeyCode(59	,VK_OEM_1))			 /* ; */
#define SIByL_KEY_EQUAL              (GetKeyCode(61	,VK_OEM_PLUS))			 /* = */
#define SIByL_KEY_A                  (GetKeyCode(65	,0x41))
#define SIByL_KEY_B                  (GetKeyCode(66	,0x42))
#define SIByL_KEY_C                  (GetKeyCode(67	,0x43))
#define SIByL_KEY_D                  (GetKeyCode(68	,0x44))
#define SIByL_KEY_E                  (GetKeyCode(69	,0x45))
#define SIByL_KEY_F                  (GetKeyCode(70	,0x46))
#define SIByL_KEY_G                  (GetKeyCode(71	,0x47))
#define SIByL_KEY_H                  (GetKeyCode(72	,0x48))
#define SIByL_KEY_I                  (GetKeyCode(73	,0x49))
#define SIByL_KEY_J                  (GetKeyCode(74	,0x4A))
#define SIByL_KEY_K                  (GetKeyCode(75	,0x4B))
#define SIByL_KEY_L                  (GetKeyCode(76	,0x4C))
#define SIByL_KEY_M                  (GetKeyCode(77	,0x4D))
#define SIByL_KEY_N                  (GetKeyCode(78	,0x4E))
#define SIByL_KEY_O                  (GetKeyCode(79	,0x4F))
#define SIByL_KEY_P                  (GetKeyCode(80	,0x50))
#define SIByL_KEY_Q                  (GetKeyCode(81	,0x51))
#define SIByL_KEY_R                  (GetKeyCode(82	,0x52))
#define SIByL_KEY_S                  (GetKeyCode(83	,0x53))
#define SIByL_KEY_T                  (GetKeyCode(84	,0x54))
#define SIByL_KEY_U                  (GetKeyCode(85	,0x55))
#define SIByL_KEY_V                  (GetKeyCode(86	,0x56))
#define SIByL_KEY_W                  (GetKeyCode(87	,0x57))
#define SIByL_KEY_X                  (GetKeyCode(88	,0x58))
#define SIByL_KEY_Y                  (GetKeyCode(89	,0x59))
#define SIByL_KEY_Z                  (GetKeyCode(90	,0x5A))
#define SIByL_KEY_LEFT_BRACKET       (GetKeyCode(91	,VK_OEM_4))			 /* [ */
#define SIByL_KEY_BACKSLASH          (GetKeyCode(92	,VK_OEM_5))			 /* \ */
#define SIByL_KEY_RIGHT_BRACKET      (GetKeyCode(93	,VK_OEM_6))			 /* ] */
#define SIByL_KEY_GRAVE_ACCENT       (GetKeyCode(96	,VK_OEM_3))			 /* ` */
#define SIByL_KEY_WORLD_1            (GetKeyCode(161,VK_SPACE))			 /* non-US #1 */
#define SIByL_KEY_WORLD_2            (GetKeyCode(162,VK_SPACE))			 /* non-US #2 */

/* Function keys */
#define SIByL_KEY_ESCAPE             (GetKeyCode(256,VK_SPACE))			
#define SIByL_KEY_ENTER              (GetKeyCode(257,VK_RETURN))			
#define SIByL_KEY_TAB                (GetKeyCode(258,VK_TAB))			
#define SIByL_KEY_BACKSPACE          (GetKeyCode(259,VK_BACK))			
#define SIByL_KEY_INSERT             (GetKeyCode(260,VK_INSERT))			
#define SIByL_KEY_DELETE             (GetKeyCode(261,VK_DELETE))			
#define SIByL_KEY_RIGHT              (GetKeyCode(262,VK_RIGHT))			
#define SIByL_KEY_LEFT               (GetKeyCode(263,VK_LEFT))			
#define SIByL_KEY_DOWN               (GetKeyCode(264,VK_DOWN))			
#define SIByL_KEY_UP                 (GetKeyCode(265,VK_UP))			
#define SIByL_KEY_PAGE_UP            (GetKeyCode(266,VK_PRIOR))			
#define SIByL_KEY_PAGE_DOWN          (GetKeyCode(267,VK_NEXT))			
#define SIByL_KEY_HOME               (GetKeyCode(268,VK_HOME))			
#define SIByL_KEY_END                (GetKeyCode(269,VK_END))			
#define SIByL_KEY_CAPS_LOCK          (GetKeyCode(280,VK_CAPITAL))			
#define SIByL_KEY_SCROLL_LOCK        (GetKeyCode(281,VK_SCROLL))			
#define SIByL_KEY_NUM_LOCK           (GetKeyCode(282,VK_NUMLOCK))			
#define SIByL_KEY_PRINT_SCREEN       (GetKeyCode(283,VK_SNAPSHOT))			
#define SIByL_KEY_PAUSE              (GetKeyCode(284,VK_PAUSE))			
#define SIByL_KEY_F1                 (GetKeyCode(290,VK_F1 ))			
#define SIByL_KEY_F2                 (GetKeyCode(291,VK_F2 ))			
#define SIByL_KEY_F3                 (GetKeyCode(292,VK_F3 ))			
#define SIByL_KEY_F4                 (GetKeyCode(293,VK_F4 ))			
#define SIByL_KEY_F5                 (GetKeyCode(294,VK_F5 ))			
#define SIByL_KEY_F6                 (GetKeyCode(295,VK_F6 ))			
#define SIByL_KEY_F7                 (GetKeyCode(296,VK_F7 ))			
#define SIByL_KEY_F8                 (GetKeyCode(297,VK_F8 ))			
#define SIByL_KEY_F9                 (GetKeyCode(298,VK_F9 ))			
#define SIByL_KEY_F10                (GetKeyCode(299,VK_F10))			
#define SIByL_KEY_F11                (GetKeyCode(300,VK_F11))			
#define SIByL_KEY_F12                (GetKeyCode(301,VK_F12))			
#define SIByL_KEY_F13                (GetKeyCode(302,VK_F13))			
#define SIByL_KEY_F14                (GetKeyCode(303,VK_F14))			
#define SIByL_KEY_F15                (GetKeyCode(304,VK_F15))			
#define SIByL_KEY_F16                (GetKeyCode(305,VK_F16))			
#define SIByL_KEY_F17                (GetKeyCode(306,VK_F17))			
#define SIByL_KEY_F18                (GetKeyCode(307,VK_F18))			
#define SIByL_KEY_F19                (GetKeyCode(308,VK_F19))			
#define SIByL_KEY_F20                (GetKeyCode(309,VK_F20))			
#define SIByL_KEY_F21                (GetKeyCode(310,VK_F21))			
#define SIByL_KEY_F22                (GetKeyCode(311,VK_F22))			
#define SIByL_KEY_F23                (GetKeyCode(312,VK_F23))			
#define SIByL_KEY_F24                (GetKeyCode(313,VK_F24))			
#define SIByL_KEY_F25                (GetKeyCode(314,0x88))			
#define SIByL_KEY_KP_0               (GetKeyCode(320,VK_NUMPAD0))			
#define SIByL_KEY_KP_1               (GetKeyCode(321,VK_NUMPAD1))			
#define SIByL_KEY_KP_2               (GetKeyCode(322,VK_NUMPAD2))			
#define SIByL_KEY_KP_3               (GetKeyCode(323,VK_NUMPAD3))			
#define SIByL_KEY_KP_4               (GetKeyCode(324,VK_NUMPAD4))			
#define SIByL_KEY_KP_5               (GetKeyCode(325,VK_NUMPAD5))			
#define SIByL_KEY_KP_6               (GetKeyCode(326,VK_NUMPAD6))			
#define SIByL_KEY_KP_7               (GetKeyCode(327,VK_NUMPAD7))			
#define SIByL_KEY_KP_8               (GetKeyCode(328,VK_NUMPAD8))			
#define SIByL_KEY_KP_9               (GetKeyCode(329,VK_NUMPAD9))			
#define SIByL_KEY_KP_DECIMAL         (GetKeyCode(330,VK_DECIMAL))			
#define SIByL_KEY_KP_DIVIDE          (GetKeyCode(331,VK_DIVIDE))			
#define SIByL_KEY_KP_MULTIPLY        (GetKeyCode(332,VK_MULTIPLY))			
#define SIByL_KEY_KP_SUBTRACT        (GetKeyCode(333,VK_SUBTRACT))			
#define SIByL_KEY_KP_ADD             (GetKeyCode(334,VK_ADD))			
#define SIByL_KEY_KP_ENTER           (GetKeyCode(335,VK_SPACE))			
#define SIByL_KEY_KP_EQUAL           (GetKeyCode(336,VK_SPACE))			
#define SIByL_KEY_LEFT_SHIFT         (GetKeyCode(340,VK_LSHIFT))			
#define SIByL_KEY_LEFT_CONTROL       (GetKeyCode(341,VK_LCONTROL))			
#define SIByL_KEY_LEFT_ALT           (GetKeyCode(342,VK_LMENU))			
#define SIByL_KEY_LEFT_SUPER         (GetKeyCode(343,VK_SPACE))			
#define SIByL_KEY_RIGHT_SHIFT        (GetKeyCode(344,VK_RSHIFT))			
#define SIByL_KEY_RIGHT_CONTROL      (GetKeyCode(345,VK_RCONTROL))
#define SIByL_KEY_RIGHT_ALT          (GetKeyCode(346,VK_RMENU))			
#define SIByL_KEY_RIGHT_SUPER        (GetKeyCode(347,VK_SPACE))			
#define SIByL_KEY_MENU               (GetKeyCode(348,VK_MENU))			

#define SIByL_KEY_LAST               SIByL_KEY_MENU


#define SIByL_MOUSE_BUTTON_1         (GetKeyCode(0 ,VK_LBUTTON))
#define SIByL_MOUSE_BUTTON_2         (GetKeyCode(1 ,VK_RBUTTON))
#define SIByL_MOUSE_BUTTON_3         (GetKeyCode(2 ,VK_MBUTTON))
#define SIByL_MOUSE_BUTTON_4         (GetKeyCode(3 ,VK_XBUTTON1))
#define SIByL_MOUSE_BUTTON_5         (GetKeyCode(4 ,VK_XBUTTON2))
#define SIByL_MOUSE_BUTTON_6         (GetKeyCode(5 ,0x07))
#define SIByL_MOUSE_BUTTON_7         (GetKeyCode(6 ,0x07))
#define SIByL_MOUSE_BUTTON_8         (GetKeyCode(7 ,0x07))
#define SIByL_MOUSE_BUTTON_LAST      SIByL_MOUSE_BUTTON_8
#define SIByL_MOUSE_BUTTON_LEFT      SIByL_MOUSE_BUTTON_1
#define SIByL_MOUSE_BUTTON_RIGHT     SIByL_MOUSE_BUTTON_2
#define SIByL_MOUSE_BUTTON_MIDDLE    SIByL_MOUSE_BUTTON_3
