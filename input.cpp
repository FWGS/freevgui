// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "input.h"

namespace vgui
{

Cursor::Cursor(DefaultCursor dc) :
	_dc( dc )
{
	// a default cursor is a system shape, so it carries no bitmap and no
	// hotspot of its own -- whoever draws it supplies both
	_bitmap = nullptr;
	_hotspot[0] = _hotspot[1] = 0;
}

Cursor::Cursor(Bitmap *bmp, int x, int y) :
	_dc( DC_USER )
{
	privateInit( bmp, x, y );
}

void Cursor::getHotspot(int &x, int &y)
{
	x = _hotspot[0];
	y = _hotspot[1];
}

void Cursor::privateInit(Bitmap *bmp, int x, int y)
{
	_bitmap = bmp;
	_hotspot[0] = x;
	_hotspot[1] = y;
}

Bitmap *Cursor::getBitmap()
{
	return _bitmap;
}

Cursor::DefaultCursor Cursor::getDefaultCursor()
{
	return _dc;
}

}
