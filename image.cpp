// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "image.h"
#include "vgui_internal.h"
#include "app.h"
#include "panel.h"
#include "surface.h"
#include "inputstream.h"
#include "fileimage.h"
#include "font.h"

using namespace vgui;

Color::Color()
{
	init();
}

Color::Color( int r, int g, int b, int a )
{
	init();
	setColor( r, g, b, a );
}

Color::Color( Scheme::SchemeColor sc )
{
	init();
	setColor( sc );
}

void Color::init()
{
	memset( rgba, 0, sizeof( rgba ));
	schemeColor = Scheme::SC_USER;
}

void Color::setColor( int r, int g, int b, int a )
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
	schemeColor = Scheme::SC_USER;
}

void Color::setColor( Scheme::SchemeColor sc )
{
	schemeColor = sc;
}

void Color::getColor( int &r, int &g, int &b, int &a )
{
	if( schemeColor == Scheme::SC_USER )
	{
		r = rgba[0];
		g = rgba[1];
		b = rgba[2];
		a = rgba[3];
	}
	else
	{
		App::getInstance()->getScheme()->getColor( schemeColor, r, g, b, a );
	}
}

void Color::getColor( Scheme::SchemeColor &sc )
{
	sc = schemeColor;
}

int Color::operator[]( int i )
{
	int co[4];
	getColor( co[0], co[1], co[2], co[3] );

	return co[i];
}

Image::Image() : panel( nullptr )
{
	setPos( 0, 0 );
	setSize( 0, 0 );
	setColor( Color( 255, 255, 255, 0 ));
}

void Image::setPos(int x, int y)
{
	origin[0] = x;
	origin[1] = y;
}

void Image::getPos(int &x, int &y)
{
	x = origin[0];
	y = origin[1];
}

void Image::getSize(int &w, int &h)
{
	w = size[0];
	h = size[1];
}

void Image::setColor(Color c)
{
	color = c;
}

void Image::getColor(Color &c)
{
	c = color;
}

void Image::setSize(int w, int h)
{
	size[0] = w;
	size[1] = h;
}

void Image::drawSetColor( Scheme::SchemeColor sc )
{
	panel->drawSetColor( sc );
}

void Image::drawSetColor( int r, int g, int b, int a )
{
	panel->drawSetColor( r, g, b, a );
}

void Image::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	x0 += origin[0];
	y0 += origin[1];
	x1 += origin[0];
	y1 += origin[1];

	panel->drawFilledRect( x0, y0, x1, y1 );
}

void Image::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	x0 += origin[0];
	y0 += origin[1];
	x1 += origin[0];
	y1 += origin[1];

	panel->drawOutlinedRect( x0, y0, x1, y1 );
}

void Image::drawSetTextFont( Scheme::SchemeFont sf )
{
	panel->drawSetTextFont( sf );
}

void Image::drawSetTextFont( Font *font )
{
	panel->drawSetTextFont( font );
}

void Image::drawSetTextColor( Scheme::SchemeColor sc )
{
	panel->drawSetTextColor( sc );
}

void Image::drawSetTextColor( int r, int g, int b, int a )
{
	panel->drawSetTextColor( r, g, b, a );
}

void Image::drawSetTextPos( int x, int y )
{
	x += origin[0];
	y += origin[1];
	panel->drawSetTextPos( x, y );
}

void Image::drawPrintText( const char *str, int len )
{
	panel->drawPrintText( str, len );
}

void Image::drawPrintText( int x, int y, const char *str, int len )
{
	x += origin[0];
	y += origin[1];
	panel->drawPrintText( x, y, str, len );
}

void Image::drawPrintChar( char ch )
{
	panel->drawPrintChar( ch );
}

void Image::drawPrintChar( int x, int y, char ch )
{
	x += origin[0];
	y += origin[1];
	panel->drawPrintChar( x, y, ch );
}

void Image::drawSetTextureRGBA( int id, const char *rgba, int w, int h )
{
	panel->drawSetTextureRGBA( id, rgba, w, h );
}

void Image::drawSetTexture( int id )
{
	panel->drawSetTexture( id );
}

void Image::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	// a1ba: missing coordinates adjust?

	panel->drawTexturedRect( x0, y0, x1, y1 );
}

void Image::paint(Panel *p)
{

}

void Image::doPaint(Panel *p)
{
	panel = p;
	paint( p );
	panel = nullptr;
}

Bitmap::Bitmap() :
	rgbaData( nullptr ),
	id( 0 ),
	uploaded( false ) {	}

void Bitmap::paint( Panel *p )
{
	if( !rgbaData )
		return;

	int wide, tall;
	getSize( wide, tall );

	if( !id )
	{
		if( p->getSurfaceBase() )
			id = p->getSurfaceBase()->createNewTextureID();
	}

	if( !uploaded )
	{
		drawSetTextureRGBA( id, (const char *)rgbaData, wide, tall );
		uploaded = true;
	}

	Color c;
	getColor( c );

	int r, g, b, a;
	c.getColor( r, g, b, a );

	drawSetTexture( id );
	drawSetColor( r, g, b, a );

	int x, y;
	getPos( x, y );

	drawTexturedRect( x, y, x + wide, y + tall );
}

void Bitmap::setSize(int wide, int tall)
{
	Image::setSize( wide, tall );
	if( rgbaData )
		delete[] rgbaData;
	rgbaData = new unsigned char[wide * tall * 4];
}

void Bitmap::setRGBA(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if( !rgbaData )
		return;

	int wide, tall;
	getSize( wide, tall );

	if( x >= 0 && x < wide && y >= 0 && y < tall )
	{
		rgbaData[( y * wide + x ) * 4 + 0] = r;
		rgbaData[( y * wide + x ) * 4 + 1] = g;
		rgbaData[( y * wide + x ) * 4 + 2] = b;
		rgbaData[( y * wide + x ) * 4 + 3] = a;
	}
}

BitmapTGA::BitmapTGA(InputStream *is, bool invertAlpha)
{
	loadTGA( is, invertAlpha );
}

bool BitmapTGA::loadTGA( InputStream *is, bool invertAlpha )
{
	if( !is )
		return false;

	DataInputStream dis( is );
	bool success = false;

#define DIS_READ( name, method ) \
	do \
	{ \
		name = dis.method( success ); \
		if( !success ) \
			return false; \
	} \
	while( 0 )

	TGAFileHeader hdr;

	DIS_READ( hdr.m_IDLength, readUChar );
	DIS_READ( hdr.m_ColorMapType, readUChar );
	DIS_READ( hdr.m_ImageType, readUChar );
	DIS_READ( hdr.m_CMapStart, readUShort );
	DIS_READ( hdr.m_CMapLength, readUShort );
	DIS_READ( hdr.m_CMapDepth, readUChar );
	DIS_READ( hdr.m_XOffset, readUShort );
	DIS_READ( hdr.m_YOffset, readUShort );
	DIS_READ( hdr.m_Width, readUShort );
	DIS_READ( hdr.m_Height, readUShort );
	DIS_READ( hdr.m_PixelDepth, readUChar );
	DIS_READ( hdr.m_ImageDescriptor, readUChar );

	if( hdr.m_ImageType != 2 && hdr.m_ImageType != 10 )
		return false;

	if( hdr.m_ColorMapType != 0 || ( hdr.m_PixelDepth != 32 && hdr.m_PixelDepth != 24 ))
		return false;

	int wide = hdr.m_Width, tall = hdr.m_Height;

	setSize( wide, tall );

	if( !rgbaData )
		return false;

	if( hdr.m_IDLength != 0 )
		dis.seekRelative( hdr.m_IDLength, success );

	if( hdr.m_ImageType == 2 )
	{
		for( int y = tall - 1; y >= 0; y-- )
		{
			unsigned char *ptr = &rgbaData[y * wide * 4];

			for( int x = 0; x < wide; x++ )
			{
				switch( hdr.m_PixelDepth )
				{
				case 24:
					DIS_READ( ptr[2], readUChar );
					DIS_READ( ptr[1], readUChar );
					DIS_READ( ptr[0], readUChar );
					ptr[3] = 255; // a1ba: it's missing invertAlpha check? O_o?
					break;
				case 32:
					DIS_READ( ptr[2], readUChar );
					DIS_READ( ptr[1], readUChar );
					DIS_READ( ptr[0], readUChar );
					DIS_READ( ptr[3], readUChar );
					if( !invertAlpha )
						ptr[3] = 255 - ptr[3];
					break;
				}

				ptr += 4;
			}
		}
	}
	else
	{
		for( int y = tall - 1; y >= 0; y-- )
		{
			unsigned char *ptr = &rgbaData[y * wide * 4];

			for( int x = 0; x < wide; x++ )
			{
				unsigned char pkthdr, pktsize;

				DIS_READ( pkthdr, readUChar );
				pktsize = ( pkthdr & 0x7f ) + 1;

				if( pkthdr & 0x80 )
				{
					unsigned char pixel[4];

					switch( hdr.m_PixelDepth )
					{
					case 24:
						DIS_READ( pixel[2], readUChar );
						DIS_READ( pixel[1], readUChar );
						DIS_READ( pixel[0], readUChar );
						pixel[3] = invertAlpha ? 0 : 255;
						break;
					case 32:
						DIS_READ( pixel[2], readUChar );
						DIS_READ( pixel[1], readUChar );
						DIS_READ( pixel[0], readUChar );
						DIS_READ( pixel[3], readUChar );
						if( !invertAlpha )
							ptr[3] = 255 - ptr[3];
						break;
					}

					for( int j = 0; j < pktsize; j++ )
					{
						y++;
						memcpy( ptr, pixel, sizeof( pixel ));
						ptr += 4;
						if( y == wide )
						{
							if( x == 0 )
								goto quickexit;

							x--;
							ptr = &rgbaData[y * x * 4];
							y = 0;
						}
					}
				}
				else
				{
					for( int j = 0; j < pktsize; j++ )
					{
						switch( hdr.m_PixelDepth )
						{
						case 24:
							DIS_READ( ptr[2], readUChar );
							DIS_READ( ptr[1], readUChar );
							DIS_READ( ptr[0], readUChar );
							ptr[3] = invertAlpha ? 0 : 255;
							break;
						case 32:
							DIS_READ( ptr[2], readUChar );
							DIS_READ( ptr[1], readUChar );
							DIS_READ( ptr[0], readUChar );
							DIS_READ( ptr[3], readUChar );
							if( !invertAlpha )
								ptr[3] = 255 - ptr[3];
							break;
						}

						y++;
						if( y == wide )
						{
							if( x == 0 )
								goto quickexit;
							x--;
							ptr = &rgbaData[x * y * 4];
							y = 0;
						}
					}
				}

				ptr += 4;
			}
		}
quickexit:
		;
	}

	return true;
}

TextImage::TextImage(int len, const char *str)
{
	init( len, str );
}

TextImage::TextImage(const char *str)
{
	init( strlen( str ) + 1, str );
}

void TextImage::paint( Panel *p )
{
	int w, h;
	getSize( w, h );

	if( text == nullptr )
		return;

	Color c;
	getColor( c );

	{
		int r, g, b, a;
		c.getColor( r, g, b, a );
		drawSetTextColor( r, g, b, a );
	}

	Font *f = getFont();
	drawSetTextFont( f );

	int x = 0, y = 0, tall = f->getTall();
	for( int i = 0; ; i++ )
	{
		int ch = text[i];

		int a, b, c;
		f->getCharABCwide( ch, a, b, c );
		int len = a + b + c;

		if( !ch )
			break;

		if( ch == '\r' )
			continue;

		if( ch == '\n' )
		{
			x = 0;
			y += tall;
			continue;
		}

		if( ch == ' ' )
		{
			int ch2 = text[i + 1];

			f->getCharABCwide( ' ', a, b, c );

			if( ch2 != '\0' && ch2 != '\n' && ch2 != '\r' )
			{
				x += a + b + c;
				if( x > w )
				{
					x = 0;
					y += tall;
				}
			}

			continue;
		}

		int word;
		for( word = 1; ; word++ )
		{
			int ch2 = text[i + word];

			if( ch2 == '\0' || ch2 == '\n' || ch2 == '\r' || ch2 == ' ' )
				break;

			f->getCharABCwide( ch2, a, b, c );
			len += a + b + c;
		}

		if( x + len > w )
		{
			x = 0;
			y += tall;
		}

		for( int j = 0; j < word; j++ )
		{
			ch = text[i + j];

			f->getCharABCwide( ch, a, b, c );
			drawPrintChar( x, y, ch );

			x += a + b + c;
		}

		i += word - 1;
	}
}

void TextImage::setSize(int w, int h)
{
	Image::setSize( w, h );
	// size[0] = w;
	// size[1] = h;
}

void TextImage::init(int len, const char *str)
{
	schemeFont = Scheme::SF_PRIMARY1;
	text = nullptr;
	font = nullptr;
	textBufferSize = 0;
	setText( str ); // ???

	int w, h;
	getTextSize( w, h );
	setSize( w, h );
}

void TextImage::getTextSize( int &w, int &h )
{
	w = 0;
	h = 0;

	if( text == nullptr )
		return;

	Font *f = getFont();
	if( f == nullptr )
		return;

	f->getTextSize( text, w, h );
}

void TextImage::getTextSizeWrapped( int &wide, int &tall )
{
	// a1ba: maybe not accurate, based on paint() code

	wide = 0;
	tall = 0;

	if( text == nullptr )
		return;

	int wrapWide, wrapTall;
	getSize( wrapWide, wrapTall );

	Font *f = getFont();
	int x = 0, y = 0, lineTall = f->getTall();

	for( int i = 0; ; i++ )
	{
		int ch = text[i];

		int a, b, c;
		f->getCharABCwide( ch, a, b, c );
		int len = a + b + c;

		if( !ch )
			break;

		if( ch == '\r' )
			continue;

		if( ch == '\n' )
		{
			x = 0;
			y += lineTall;
			continue;
		}

		if( ch == ' ' )
		{
			int ch2 = text[i + 1];

			f->getCharABCwide( ' ', a, b, c );

			if( ch2 != '\0' && ch2 != '\n' && ch2 != '\r' )
			{
				x += a + b + c;
				if( x > wrapWide )
				{
					x = 0;
					y += lineTall;
				}
			}

			continue;
		}

		int word;
		for( word = 1; ; word++ )
		{
			int ch2 = text[i + word];

			if( ch2 == '\0' || ch2 == '\n' || ch2 == '\r' || ch2 == ' ' )
				break;

			f->getCharABCwide( ch2, a, b, c );
			len += a + b + c;
		}

		if( x + len > wrapWide )
		{
			x = 0;
			y += lineTall;
		}

		for( int j = 0; j < word; j++ )
		{
			ch = text[i + j];

			f->getCharABCwide( ch, a, b, c );

			if( x + a + b + c > wide )
				wide = x + a + b + c;

			if( y + lineTall > tall )
				tall = y + lineTall;

			x += a + b + c;
		}

		i += word - 1;
	}
}

Font *TextImage::getFont()
{
	if( font )
		return font;

	return App::getInstance()->getScheme()->getFont( schemeFont );
}

void TextImage::setText( int len, const char *str )
{
	if( len > textBufferSize )
	{
		delete[] text;
		textBufferSize = len;
		text = new char[len];

		if( !text )
			textBufferSize = 0;
	}

	if( text )
		vgui_strcpy( text, textBufferSize, str );

	int w, h;
	getTextSize( w, h );
	Image::setSize( w, h );
}

void TextImage::setText(const char *str)
{
	setText( strlen( str ) + 1, str );
}

void TextImage::setFont(Scheme::SchemeFont sf)
{
	schemeFont = sf;
}

void TextImage::setFont(Font *f)
{
	font = f;
}

EtchedBorder::EtchedBorder() : Border()
{
	setInset( 2, 2, 2, 2 );
}

void EtchedBorder::paint( Panel *p )
{
	int w, h;

	p->getSize( w, h );

	drawSetColor( Scheme::SC_WHITE );
	drawFilledRect( 0,     0,     w, 2 );
	drawFilledRect( 0,     h - 2, w, h );
	drawFilledRect( 0,     2,     2, h - 2 );
	drawFilledRect( w - 2, 2,     w, h - 2 );

	drawSetColor( Scheme::SC_SECONDARY2 );
	drawFilledRect(0,     0,     w - 1, 1 );
	drawFilledRect(0,     h - 2, w - 1, h - 1 );
	drawFilledRect(0,     1,     1,     h - 2 );
	drawFilledRect(w - 2, 1,     w - 1, h - 2 );
}

LineBorder::LineBorder(int thickness, Color c)
{
	init( thickness, c );
}

LineBorder::LineBorder(int thickness) :
	LineBorder( thickness, Color( 0, 0, 0, 0 ))
{

}

LineBorder::LineBorder(Color c) :
	LineBorder( 1, c )
{

}

LineBorder::LineBorder() : LineBorder( 1 )
{

}

void LineBorder::paint( Panel *p )
{
	int w, h;

	p->getSize( w, h );

	int r, g, b, a;
	color.getColor( r, g, b, a );
	drawSetColor( r, g, b, a );

	drawFilledRect( 0, 0, w, inset[1] );
	drawFilledRect( 0, h - inset[3], w, h );
	drawFilledRect( 0, inset[1], inset[0], h - inset[3] );
	drawFilledRect( w - inset[2], inset[1], w, h - inset[3] );
}

void LineBorder::init(int thickness, Color c)
{
	setInset( thickness, thickness, thickness, thickness );
	color = c;
}

LoweredBorder::LoweredBorder() : Border()
{
	setInset( 2, 2, 2, 2 );
}

void LoweredBorder::paint( Panel *p )
{
	int w, h;

	p->getSize( w, h );
	drawSetColor( Scheme::SC_SECONDARY2 );
	drawFilledRect( 0, 0, w, 2 );
	drawFilledRect( 0, 2, 2, h );

	drawSetColor( Scheme::SC_WHITE );
	drawFilledRect( 1, h - 2, w, h );
	drawFilledRect( w - 2, 1, w, h - 1 );

	drawSetColor( Scheme::SC_SECONDARY1 );
	drawFilledRect( 1, 1, w - 1, 2 );
	drawFilledRect( 1, 2, 2, h - 1 );
}

RaisedBorder::RaisedBorder() : Border()
{
	setInset( 2, 2, 2, 2 );
}

void RaisedBorder::paint( Panel *p )
{
	int w, h;

	p->getSize( w, h );
	drawSetColor( Scheme::SC_WHITE );
	drawFilledRect( 0, 0, w, 2 );
	drawFilledRect( 0, 2, 2, h );

	drawSetColor( Scheme::SC_SECONDARY2 );
	drawFilledRect( 2, h - 2, w, h );
	drawFilledRect( w - 2, 2, w, h - 1 );

	drawSetColor( Scheme::SC_SECONDARY1 );
	drawFilledRect( 1, h - 1, w, h );
	drawFilledRect( w - 1, 1, w, h - 1 );
}

Border::Border(int left, int top, int right, int bottom)
{
	panel = nullptr;
	inset[0] = left;
	inset[1] = top;
	inset[2] = right;
	inset[3] = bottom;
}

void Border::setInset(int left, int top, int right, int bottom)
{
	inset[0] = left;
	inset[1] = top;
	inset[2] = right;
	inset[3] = bottom;
}

void Border::getInset(int &left, int &top, int &right, int &bottom)
{
	left = inset[0];
	top = inset[1];
	right = inset[2];
	bottom = inset[3];
}

void Border::drawFilledRect(int x0, int y0, int x1, int y1)
{
	Image::drawFilledRect( x0 - inset[0], y0 - inset[1], x1 - inset[0], y1 - inset[1] );
}

void Border::drawOutlinedRect(int x0, int y0, int x1, int y1)
{
	Image::drawOutlinedRect( x0 - inset[0], y0 - inset[1], x1 - inset[0], y1 - inset[1] );
}

void Border::drawSetTextPos(int x, int y)
{
	Image::drawSetTextPos( x - inset[0], y - inset[1] );
}

void Border::drawPrintText(int x, int y, const char *str, int len)
{
	Image::drawPrintText( x - inset[0], y - inset[1], str, len );
}

void Border::drawPrintChar(int x, int y, char ch)
{
	Image::drawPrintChar( x - inset[0], y - inset[1], ch );
}

BorderPair::BorderPair(Border *border1, Border *border2)
{
	borders[0] = border1;
	borders[1] = border2;
}

void BorderPair::doPaint(Panel *p)
{
	if( borders[0] )
		borders[0]->doPaint( p );
	if( borders[1] )
		borders[1]->doPaint( p );
}

void BorderPair::paint(Panel *p)
{
	if( borders[0] )
		borders[0]->paint( p );
	if( borders[1] )
		borders[1]->paint( p );
}
