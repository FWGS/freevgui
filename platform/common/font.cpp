// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "platform/common/font.h"
#include "vgui_internal.h"

using namespace vgui;

// the sheet is a single row of cells, one per byte value
static constexpr int FONT_SHEET_CELLS = 256;

FontPlat_Bitmap::FontPlat_Bitmap() : name( nullptr ), bitmap( nullptr ), cellWide( 0 ), cellTall( 0 )
{
	memset( charWidths, 0, sizeof( charWidths ));
}

FontPlat_Bitmap::~FontPlat_Bitmap()
{
	delete[] bitmap;
	delete[] name;
}

bool FontPlat_Bitmap::loadFrom32BitTGA( FileImageStream *stream )
{
	FileImage fileImage;

	if( !Load32BitTGA( stream, &fileImage ))
		return false;

	cellWide = fileImage.m_Width / FONT_SHEET_CELLS;
	cellTall = fileImage.m_Height;

	bitmap = new unsigned char[fileImage.m_Width * fileImage.m_Height];

	if( !bitmap )
		return false;

	memset( bitmap, 0, fileImage.m_Width * fileImage.m_Height );

	for( int ch = 0; ch < FONT_SHEET_CELLS; ch++ )
	{
		const unsigned char *in = &fileImage.m_pData[ch * cellWide * 4];
		unsigned char *out = &bitmap[ch * cellWide];
		int rightX = 0;

		for( int y = 0; y < cellTall; y++ )
		{
			for( int x = 0; x < cellWide; x++ )
			{
				// any non-zero channel counts as ink, so the sheet can be drawn in any color and still measure the same
				if( in[x * 4] || in[x * 4 + 1] || in[x * 4 + 2] || in[x * 4 + 3] )
				{
					out[x] = 1;

					if( x > rightX )
						rightX = x;
				}
				else out[x] = 0;
			}

			// next row of the sheet, which is a whole row of cells away
			in += FONT_SHEET_CELLS * cellWide * 4;
			out += FONT_SHEET_CELLS * cellWide;
		}

		// space gets a quarter cell width
		if( ch == ' ' )
			charWidths[ch] = cellWide / 4;
		else
			charWidths[ch] = rightX;
	}

	return true;
}

FontPlat_Bitmap *FontPlat_Bitmap::Create( const char *name, FileImageStream *stream )
{
	FontPlat_Bitmap *font = new FontPlat_Bitmap();

	if( !font )
		return nullptr;

	if( !font->loadFrom32BitTGA( stream ))
		goto err;

	font->name = vgui_strdup( name );

	if( !font->name )
		goto err;

	return font;

err:
	delete font;
	return nullptr;
}

bool FontPlat_Bitmap::equals( const char *, int, int, float, int, bool, bool, bool, bool )
{
	// sheets are matched by the file they came from, never by these parameters
	return false;
}

void FontPlat_Bitmap::getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
	ch = bound( 0, ch, FONT_SHEET_CELLS - 1 );

	for( int y = 0; y < cellTall; y++ )
	{
		const unsigned char *src = &bitmap[cellWide * ( ch + y * FONT_SHEET_CELLS )];

		for( int x = 0; x < cellWide; x++ )
		{
			int outX = rgbaX + x, outY = rgbaY + y;

			if( outX < rgbaWide && outY < rgbaTall )
				memset( &rgba[( outY * rgbaWide + outX ) * 4], src[x] ? 255 : 0, 4 );
		}
	}
}

void FontPlat_Bitmap::getCharABCwide( int ch, int &a, int &b, int &c )
{
	ch = bound( 0, ch, FONT_SHEET_CELLS - 1 );

	a = c = 0;
	b = charWidths[ch] + 1;
}

int FontPlat_Bitmap::getTall()
{
	return cellTall;
}

int FontPlat_Bitmap::getWide()
{
	return cellWide;
}

void FontPlat_Bitmap::drawSetTextFont( SurfacePlat * )
{
	// stub
}
