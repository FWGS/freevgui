// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "font.h"
#include "platform/common/font.h"
#include "fileimage.h"

using namespace vgui;

static int staticFontId = 100;
static Dar<vgui::BaseFontPlat*> staticFontPlatDar;

class StubFontPlat : public BaseFontPlat
{
public:
	StubFontPlat( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol ) :
		tall( tall ), wide( wide )
	{
	}

	virtual bool equals( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
	{
		return this->tall == tall && this->wide == wide;
	}

	virtual void getCharRGBA( int, int, int, int, int, unsigned char * )
	{
	}

	virtual void getCharABCwide( int ch , int &a, int &b, int &c )
	{
		a = b = c = 0;
	}

	virtual int getTall()
	{
		return tall;
	}

	virtual int getWide()
	{
		return wide;
	}

	virtual void drawSetTextFont( SurfacePlat* )
	{
	}

private:
	int tall, wide;
};

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

void Font::init( const char *newName, void *pFileData, int fileDataLen, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	name = vgui_strdup( newName );
	id = -1;
	impl = nullptr;

	if( pFileData )
	{
		FileImageStream_Memory stream( pFileData, fileDataLen );
		FontPlat_Bitmap *bitmap = FontPlat_Bitmap::Create( name, &stream );

		if( bitmap )
		{
			impl = bitmap;
			staticFontPlatDar.addElement( impl );
			id = staticFontId++;
		}
	}
	else
	{
		for( int i = 0; i < staticFontPlatDar.getCount(); i++ )
		{
			if( staticFontPlatDar[i]->equals( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol ))
			{
				impl = staticFontPlatDar[i];
				break;
			}
		}

		if( !impl )
		{
			impl = FontPlat_CreateSystem( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol );

			if( !impl )
				impl = new StubFontPlat( name, tall, wide, rotation, weight, italic, underline, strikeout, symbol );

			staticFontPlatDar.addElement( impl );
			id = staticFontId++;
		}
	}
}

void Font::getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
	impl->getCharRGBA( ch, rgbaX, rgbaY, rgbaWide, rgbaTall, rgba );
}

void Font::getCharABCwide( int ch, int &a, int &b, int &c )
{
	impl->getCharABCwide( ch, a, b, c );
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
	return impl;
}

int Font::getTall()
{
	return impl->getTall();
}

#ifndef _WIN32
int Font::getWide()
{
	return impl->getWide();
}
#endif // _WIN32

int Font::getId()
{
	return id;
}
