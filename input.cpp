// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "input.h"

namespace vgui
{

Cursor::Cursor(DefaultCursor dc) :
	defaultCursor( dc )
{
	// a default cursor is a system shape, so it carries no bitmap and no
	// hotspot of its own -- whoever draws it supplies both
	bitmap = nullptr;
	hotspot[0] = hotspot[1] = 0;
}

Cursor::Cursor(Bitmap *bmp, int x, int y) :
	defaultCursor( DC_USER )
{
	privateInit( bmp, x, y );
}

void Cursor::getHotspot(int &x, int &y)
{
	x = hotspot[0];
	y = hotspot[1];
}

void Cursor::privateInit(Bitmap *bmp, int x, int y)
{
	bitmap = bmp;
	hotspot[0] = x;
	hotspot[1] = y;
}

Bitmap *Cursor::getBitmap()
{
	return bitmap;
}

Cursor::DefaultCursor Cursor::getDefaultCursor()
{
	return defaultCursor;
}

}
