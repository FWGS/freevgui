// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2017-2026 Alibek Omarov

#ifndef PLATFORM_WIN32_FONT_H
#define PLATFORM_WIN32_FONT_H

#include "platform/common/font.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace vgui
{

// system font rasterised through WinAPI
// glyphs are produced one at a time into a caller-supplied RGBA buffer, as the cache is owned by surface
class FontPlat_Win32 : public BaseFontPlat
{
public:
	// returns null on error
	static FontPlat_Win32 *Create( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol );

	virtual ~FontPlat_Win32() override;

	virtual bool equals( const char *newName, int newTall, int newWide, float newRotation, int newWeight, bool newItalic, bool newUnderline, bool newStrikeout, bool newSymbol ) override;
	virtual void getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba ) override;
	virtual void getCharABCwide( int ch, int &a, int &b, int &c ) override;
	virtual int getTall() override;
	virtual int getWide() override;
	virtual void drawSetTextFont( SurfacePlat * ) override;

private:
	FontPlat_Win32();

	bool init( const char *newName, int newTall, int newWide, float newRotation, int newWeight, bool newItalic, bool newUnderline, bool newStrikeout, bool newSymbol );
	void blendGlyphPixel( unsigned char *rgba, int rgbaWide, int rgbaTall, int x, int y, unsigned char coverage );

	// the request, kept verbatim so equals() can answer without re-querying GDI
	char *name;
	int   tall, wide, weight;
	float rotation;
	bool  italic, underline, strikeout, symbol;

	HDC     dc;
	HFONT   font;
	HBITMAP dib;

	// owned by the DIB section, not separately allocated
	unsigned char *dibPixels;
	int dibSize[2];

	// metrics cached from GetTextMetricsW
	int height, maxCharWidth;
};
}

#endif // PLATFORM_WIN32_FONT_H
