#pragma once

#include "KeyCodes.h"
/*! @} */

/*! @defgroup buttons Mouse buttons
 *  @brief Mouse button IDs.
 *
 *  See [mouse button input](@ref input_mouse_button) for how these are used.
 *
 *  @ingroup input
 *  @{ */
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
 /*! @} */