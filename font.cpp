// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "font.h"
#include "platform/common/font.h"
#include "platform/common/fileimage.h"

using namespace vgui;

static int staticFontId = 100;
static Dar<vgui::BaseFontPlat*> staticFontPlatDar;

void vgui::Font_Reset( void )
{
	staticFontPlatDar.setCount( 0 );
}

Font::Font( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	init( name, nullptr, 0, tall, wide, rotation, weight, italic, underline, strikeout, symbol );
}

Font::Font( const char *name, void *pFileData, int fileDataLen, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	init( name, pFileData, fileDataLen, tall, wide, rotation, weight, italic, underline, strikeout, symbol );
}

void Font::init( const char *name, void *pFileData, int fileDataLen, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	_name = vgui_strdup( name );
	_id = -1;
	_plat = nullptr;

	if( pFileData )
	{
		FileImageStream_Memory stream( pFileData, fileDataLen );
		FontPlat_Bitmap *bitmap = FontPlat_Bitmap::Create( _name, &stream );

		if( bitmap )
		{
			_plat = bitmap;
			staticFontPlatDar.addElement( _plat );
			_id = staticFontId++;
		}
	}
	else
	{
		for( int i = 0; i < staticFontPlatDar.getCount(); i++ )
		{
			if( staticFontPlatDar[i]->equals( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol ))
			{
				_plat = staticFontPlatDar[i];
				break;
			}
		}

		if( !_plat )
		{
			_plat = new FontPlat( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol );
			staticFontPlatDar.addElement( _plat );
			_id = staticFontId++;
		}
	}
}

void Font::getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
	_plat->getCharRGBA( ch, rgbaX, rgbaY, rgbaWide, rgbaTall, rgba );
}

void Font::getCharABCwide( int ch, int &a, int &b, int &c )
{
	_plat->getCharABCwide( ch, a, b, c );
}

void Font::getTextSize( const char *str, int &wide, int &tall )
{
	wide = 0;
	tall = 0;

	if( !str )
		return;

	tall = getTall();

	for( int i = 0, xx = 0;; i++ )
	{
		char ch = str[i];

		if( !ch )
			break;

		if( ch == '\n' )
		{
			tall += getTall();
			xx = 0;
		}

		int a, b, c;
		getCharABCwide( ch, a, b, c );
		xx += a + b + c;

		if( xx > wide )
			wide = xx;
	}
}

BaseFontPlat *Font::getPlat()
{
	return _plat;
}

int Font::getTall()
{
	return _plat->getTall();
}

int Font::getWide()
{
	return _plat->getWide();
}

int Font::getId()
{
	return _id;
}
