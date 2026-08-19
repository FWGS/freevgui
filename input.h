// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_INPUT_H
#define VGUI_INPUT_H

#include "vgui.h"

namespace vgui
{
class Bitmap;

enum MouseCode : int32_t
{
	MOUSE_LEFT   = 0,
	MOUSE_RIGHT  = 1,
	MOUSE_MIDDLE = 2,
	MOUSE_LAST   = 3,
};

enum KeyCode : int32_t
{
	KEY_0            = 0,
	KEY_1            = 1,
	KEY_2            = 2,
	KEY_3            = 3,
	KEY_4            = 4,
	KEY_5            = 5,
	KEY_6            = 6,
	KEY_7            = 7,
	KEY_8            = 8,
	KEY_9            = 9,
	KEY_A            = 10,
	KEY_B            = 11,
	KEY_C            = 12,
	KEY_D            = 13,
	KEY_E            = 14,
	KEY_F            = 15,
	KEY_G            = 16,
	KEY_H            = 17,
	KEY_I            = 18,
	KEY_J            = 19,
	KEY_K            = 20,
	KEY_L            = 21,
	KEY_M            = 22,
	KEY_N            = 23,
	KEY_O            = 24,
	KEY_P            = 25,
	KEY_Q            = 26,
	KEY_R            = 27,
	KEY_S            = 28,
	KEY_T            = 29,
	KEY_U            = 30,
	KEY_V            = 31,
	KEY_W            = 32,
	KEY_X            = 33,
	KEY_Y            = 34,
	KEY_Z            = 35,
	KEY_PAD_0        = 36,
	KEY_PAD_1        = 37,
	KEY_PAD_2        = 38,
	KEY_PAD_3        = 39,
	KEY_PAD_4        = 40,
	KEY_PAD_5        = 41,
	KEY_PAD_6        = 42,
	KEY_PAD_7        = 43,
	KEY_PAD_8        = 44,
	KEY_PAD_9        = 45,
	KEY_PAD_DIVIDE   = 46,
	KEY_PAD_MULTIPLY = 47,
	KEY_PAD_MINUS    = 48,
	KEY_PAD_PLUS     = 49,
	KEY_PAD_ENTER    = 50,
	KEY_PAD_DECIMAL  = 51,
	KEY_LBRACKET     = 52,
	KEY_RBRACKET     = 53,
	KEY_SEMICOLON    = 54,
	KEY_APOSTROPHE   = 55,
	KEY_BACKQUOTE    = 56,
	KEY_COMMA        = 57,
	KEY_PERIOD       = 58,
	KEY_SLASH        = 59,
	KEY_BACKSLASH    = 60,
	KEY_MINUS        = 61,
	KEY_EQUAL        = 62,
	KEY_ENTER        = 63,
	KEY_SPACE        = 64,
	KEY_BACKSPACE    = 65,
	KEY_TAB          = 66,
	KEY_CAPSLOCK     = 67,
	KEY_NUMLOCK      = 68,
	KEY_ESCAPE       = 69,
	KEY_SCROLLLOCK   = 70,
	KEY_INSERT       = 71,
	KEY_DELETE       = 72,
	KEY_HOME         = 73,
	KEY_END          = 74,
	KEY_PAGEUP       = 75,
	KEY_PAGEDOWN     = 76,
	KEY_BREAK        = 77,
	KEY_LSHIFT       = 78,
	KEY_RSHIFT       = 79,
	KEY_LALT         = 80,
	KEY_RALT         = 81,
	KEY_LCONTROL     = 82,
	KEY_RCONTROL     = 83,
	KEY_LWIN         = 84,
	KEY_RWIN         = 85,
	KEY_APP          = 86,
	KEY_UP           = 87,
	KEY_LEFT         = 88,
	KEY_DOWN         = 89,
	KEY_RIGHT        = 90,
	KEY_F1           = 91,
	KEY_F2           = 92,
	KEY_F3           = 93,
	KEY_F4           = 94,
	KEY_F5           = 95,
	KEY_F6           = 96,
	KEY_F7           = 97,
	KEY_F8           = 98,
	KEY_F9           = 99,
	KEY_F10          = 100,
	KEY_F11          = 101,
	KEY_F12          = 102,
	KEY_LAST         = 103,
};

class CLASSEXPORT Cursor
{
public:
	enum DefaultCursor : int32_t
	{
		DC_USER = 0, DC_NONE, DC_ARROW, DC_IBEAM, DC_HOURGLASS,
		DC_CROSSHAIR, DC_UP, DC_SIZENWSE, DC_SIZENESW, DC_SIZEWE, DC_SIZENS,
		DC_SIZEALL, DC_NO, DC_HAND, DC_LAST
	};

	Cursor( DefaultCursor dc );
	Cursor( Bitmap *bmp, int x, int y );

	virtual void getHotspot( int &x, int &y );

private:
	virtual void privateInit( Bitmap *bmp, int x, int y );

	int           _hotspot[2];
	Bitmap*       _bitmap;
	DefaultCursor _dc;
public:
	virtual Bitmap* getBitmap();
	virtual DefaultCursor getDefaultCursor();
};

CHECK_STRUCT_SIZE( Cursor, 20, 32, 32 );

}

#endif // VGUI_INPUT_H
