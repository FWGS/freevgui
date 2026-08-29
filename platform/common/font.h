// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef PLATFORM_COMMON_FONT_H
#define PLATFORM_COMMON_FONT_H
#include "fileimage.h"

namespace vgui
{
class SurfacePlat;

class BaseFontPlat
{
public:
	virtual ~BaseFontPlat() {}
	virtual bool equals( const char *, int, int, float, int, bool, bool, bool, bool ) = 0;
	virtual void getCharRGBA( int, int, int, int, int, unsigned char * ) = 0;
	virtual void getCharABCwide( int ch , int &a, int &b, int &c ) = 0;
	virtual int getTall() = 0;
	virtual int getWide() = 0;
	virtual void drawSetTextFont( SurfacePlat* ) = 0;
};

// platform-specific font implementation
BaseFontPlat *FontPlat_CreateSystem( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol );

// a font sheet: one TGA holding 256 fixed-size cells side by side, decoded down to one byte of coverage per pixel
class FontPlat_Bitmap : public BaseFontPlat
{
public:
	static FontPlat_Bitmap *Create( const char *name, FileImageStream *stream );

	virtual ~FontPlat_Bitmap() override;
	virtual bool equals( const char *, int, int, float, int, bool, bool, bool, bool ) override;
	virtual void getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba ) override;
	virtual void getCharABCwide( int ch , int &a, int &b, int &c ) override;
	virtual int getTall() override;
	virtual int getWide() override;
	virtual void drawSetTextFont( SurfacePlat* ) override;

private:
	FontPlat_Bitmap();

	bool loadFrom32BitTGA( FileImageStream *stream );

	char *name;

	// coverage, one byte per pixel: the cells stay side by side as they were in the sheet, so glyph `ch` pixel (x,y) is at bitmap[cellWide * ( ch + y * 256 ) + x].
	unsigned char *bitmap;

	// size of one cell; every glyph occupies a whole cell
	int cellWide, cellTall;

	// per-glyph ink extent, which is what the advance is derived from
	int charWidths[256];
};
}

#endif // PLATFORM_COMMON_FONT_H
