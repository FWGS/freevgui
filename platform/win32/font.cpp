// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2017-2026 Alibek Omarov

#include "platform/win32/font.h"
#include "vgui_internal.h"

using namespace vgui;

FontPlat_Win32::FontPlat_Win32() : name( nullptr ), tall( 0 ), wide( 0 ), weight( 0 ),
	rotation( 0.0f ),
	italic( false ), underline( false ), strikeout( false ), symbol( false ),
	dc( nullptr ), font( nullptr ), dib( nullptr ),
	dibPixels( nullptr ),
	height( 0 ), maxCharWidth( 0 )
{
	dibSize[0] = dibSize[1] = 0;
}

FontPlat_Win32::~FontPlat_Win32()
{
	// the DC holds the font and the bitmap selected; drop them first
	if( dc )
		::DeleteDC( dc );

	if( font )
		::DeleteObject( font );

	if( dib )
		::DeleteObject( dib );

	delete[] name;
}

FontPlat_Win32 *FontPlat_Win32::Create( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	if( !name )
		return nullptr;

	FontPlat_Win32 *font = new FontPlat_Win32();

	if( !font )
		return nullptr;

	if( !font->init( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol ))
	{
		delete font;
		return nullptr;
	}

	return font;
}

bool FontPlat_Win32::init( const char *newName, int newTall, int newWide, float newRotation, int newWeight, bool newItalic, bool newUnderline, bool newStrikeout, bool newSymbol )
{
	name = vgui_strdup( newName );

	if( !name )
		return false;

	tall = newTall;
	wide = newWide;
	rotation = newRotation;
	weight = newWeight;
	italic = newItalic;
	underline = newUnderline;
	strikeout = newStrikeout;
	symbol = newSymbol;

	dc = ::CreateCompatibleDC( nullptr );

	if( !dc )
		return false;

	// escapement and orientation are in tenths of a degree
	int escapement = (int)( rotation * 10.0f );

	font = ::CreateFontA( tall, wide, escapement, escapement, weight, italic, underline, strikeout,
		symbol ? SYMBOL_CHARSET : DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, name );

	if( !font )
	{
		vgui_dprintf( "vgui: CreateFont failed for \"%s\"\n", name );
		return false;
	}

	::SetMapMode( dc, MM_TEXT );
	::SelectObject( dc, font );
	::SetTextAlign( dc, TA_LEFT | TA_TOP | TA_UPDATECP );

	::TEXTMETRICW tm = { 0 };

	if( !::GetTextMetricsW( dc, &tm ))
	{
		vgui_dprintf( "vgui: GetTextMetrics failed for \"%s\"\n", name );
		return false;
	}

	height = tm.tmHeight;
	maxCharWidth = tm.tmMaxCharWidth;

	// scratch surface for the ExtTextOut fallback below. Top-down (negative height) so its rows match the RGBA buffer we copy into.
	dibSize[0] = maxCharWidth;
	dibSize[1] = height;

	::BITMAPINFOHEADER header = { 0 };
	header.biSize = sizeof( header );
	header.biWidth = dibSize[0];
	header.biHeight = -dibSize[1];
	header.biPlanes = 1;
	header.biBitCount = 32;
	header.biCompression = BI_RGB;

	dib = ::CreateDIBSection( dc, (BITMAPINFO *)&header, DIB_RGB_COLORS, (void **)&dibPixels, nullptr, 0 );

	if( !dib || !dibPixels )
	{
		vgui_dprintf( "vgui: CreateDIBSection failed for \"%s\"\n", name );
		return false;
	}

	::SelectObject( dc, dib );

	return true;
}

bool FontPlat_Win32::equals( const char *newName, int newTall, int newWide, float newRotation, int newWeight, bool newItalic, bool newUnderline, bool newStrikeout, bool newSymbol )
{
	return newName && !strcmp( name, newName )
		&& tall == newTall && wide == newWide && rotation == newRotation && weight == newWeight
		&& italic == newItalic && underline == newUnderline && strikeout == newStrikeout && symbol == newSymbol;
}

void FontPlat_Win32::blendGlyphPixel( unsigned char *rgba, int rgbaWide, int rgbaTall, int x, int y, unsigned char coverage )
{
	if( x < 0 || y < 0 || x >= rgbaWide || y >= rgbaTall )
		return;

	unsigned char *dst = &rgba[( y * rgbaWide + x ) * 4];

	// white glyph, coverage in alpha -- the surface modulates by the text colour
	dst[0] = dst[1] = dst[2] = 255;
	dst[3] = coverage;
}

void FontPlat_Win32::getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
	if( !rgba || !dc || !dibPixels )
		return;

	int a, b, c;

	getCharABCwide( ch, a, b, c );

	int glyphWide = b, glyphTall = height;

	if( underline ) // the underline runs the full advance, bearings included
		glyphWide += a + c;

	if( glyphWide > dibSize[0] )
		glyphWide = dibSize[0];

	if( glyphTall > dibSize[1] )
		glyphTall = dibSize[1];

	memset( dibPixels, 0, dibSize[0] * dibSize[1] * 4 );

	::SelectObject( dc, font );
	::SetBkColor( dc, RGB( 0, 0, 0 ));
	::SetTextColor( dc, RGB( 255, 255, 255 ));
	::SetBkMode( dc, OPAQUE );

	// shift out the left bearing so the black box lands at the cell origin
	::MoveToEx( dc, underline ? 0 : -a, 0, nullptr );

	wchar_t wch = (wchar_t)ch;
	::ExtTextOutW( dc, 0, 0, 0, nullptr, &wch, 1, nullptr );
	::SetBkMode( dc, TRANSPARENT );

	for( int j = 0; j < glyphTall; j++ )
	{
		for( int i = 0; i < glyphWide; i++ )
		{
			const unsigned char *src = &dibPixels[( j * dibSize[0] + i ) * 4];
			unsigned int coverage = ( src[2] * 77 + src[1] * 151 + src[0] * 28 ) >> 8;

			blendGlyphPixel( rgba, rgbaWide, rgbaTall, rgbaX + i, rgbaY + j, (unsigned char)coverage );
		}
	}
}

void FontPlat_Win32::getCharABCwide( int ch, int &a, int &b, int &c )
{
	a = b = c = 0;

	if( !dc )
		return;

	::SelectObject( dc, font );

	ABC abc;

	if( ::GetCharABCWidthsW( dc, ch, ch, &abc ))
	{
		a = abc.abcA;
		b = abc.abcB;
		c = abc.abcC;
		return;
	}

	// raster faces have no ABC widths at all
	INT width = 0;

	if( ::GetCharWidth32W( dc, ch, ch, &width ))
	{
		b = width;
		return;
	}

	b = maxCharWidth;
}

int FontPlat_Win32::getTall()
{
	return height;
}

int FontPlat_Win32::getWide()
{
	return maxCharWidth;
}

void FontPlat_Win32::drawSetTextFont( SurfacePlat * )
{
	// stub
}

BaseFontPlat *vgui::FontPlat_CreateSystem( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	return FontPlat_Win32::Create( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol );
}
