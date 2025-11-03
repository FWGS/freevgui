// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "image.h"
#include "app.h"
#include "panel.h"
#include "surface.h"
#include "inputstream.h"
#include "platform/common/fileimage.h"
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
	memset( _color, 0, sizeof( _color ));
	_schemeColor = Scheme::SC_USER;
}

void Color::setColor( int r, int g, int b, int a )
{
	Vector4Set( _color, r, g, b, a );
	_schemeColor = Scheme::SC_USER;
}

void Color::setColor( Scheme::SchemeColor sc )
{
	_schemeColor = sc;
}

void Color::getColor( int &r, int &g, int &b, int &a )
{
	if( _schemeColor == Scheme::SC_USER )
	{
		r = _color[0];
		g = _color[1];
		b = _color[2];
		a = _color[3];
	}
	else
	{
		App::getInstance()->getScheme()->getColor( _schemeColor, r, g, b, a );
	}
}

void Color::getColor( Scheme::SchemeColor &sc )
{
	sc = _schemeColor;
}

int Color::operator[]( int i )
{
	int co[4];
	getColor( co[0], co[1], co[2], co[3] );

	return co[i];
}

Image::Image() : _panel( nullptr )
{
	setPos( 0, 0 );
	setSize( 0, 0 );
	setColor( Color( 255, 255, 255, 0 ));
}

void Image::setPos(int x, int y)
{
	_pos[0] = x;
	_pos[1] = y;
}

void Image::getPos(int &x, int &y)
{
	x = _pos[0];
	y = _pos[1];
}

void Image::getSize(int &w, int &h)
{
	w = _size[0];
	h = _size[1];
}

void Image::setColor(Color c)
{
	_color = c;
}

void Image::getColor(Color &c)
{
	c = _color;
}

void Image::setSize(int w, int h)
{
	_size[0] = w;
	_size[1] = h;
}

void Image::drawSetColor( Scheme::SchemeColor sc )
{
	_panel->drawSetColor( sc );
}

void Image::drawSetColor( int r, int g, int b, int a )
{
	_panel->drawSetColor( r, g, b, a );
}

void Image::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	x0 += _pos[0];
	y0 += _pos[1];
	x1 += _pos[0];
	y1 += _pos[1];

	_panel->drawFilledRect( x0, y0, x1, y1 );
}

void Image::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	x0 += _pos[0];
	y0 += _pos[1];
	x1 += _pos[0];
	y1 += _pos[1];

	_panel->drawOutlinedRect( x0, y0, x1, y1 );
}

void Image::drawSetTextFont( Scheme::SchemeFont sf )
{
	_panel->drawSetTextFont( sf );
}

void Image::drawSetTextFont( Font *font )
{
	_panel->drawSetTextFont( font );
}

void Image::drawSetTextColor( Scheme::SchemeColor sc )
{
	_panel->drawSetTextColor( sc );
}

void Image::drawSetTextColor( int r, int g, int b, int a )
{
	_panel->drawSetTextColor( r, g, b, a );
}

void Image::drawSetTextPos( int x, int y )
{
	x += _pos[0];
	y += _pos[1];
	_panel->drawSetTextPos( x, y );
}

void Image::drawPrintText( const char *str, int len )
{
	_panel->drawPrintText( str, len );
}

void Image::drawPrintText( int x, int y, const char *str, int len )
{
	x += _pos[0];
	y += _pos[1];
	_panel->drawPrintText( x, y, str, len );
}

void Image::drawPrintChar( char ch )
{
	_panel->drawPrintChar( ch );
}

void Image::drawPrintChar( int x, int y, char ch )
{
	x += _pos[0];
	y += _pos[1];
	_panel->drawPrintChar( x, y, ch );
}

void Image::drawSetTextureRGBA( int id, const char *rgba, int w, int h )
{
	_panel->drawSetTextureRGBA( id, rgba, w, h );
}

void Image::drawSetTexture( int id )
{
	_panel->drawSetTexture( id );
}

void Image::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	// a1ba: missing coordinates adjust?

	_panel->drawTexturedRect( x0, y0, x1, y1 );
}

void Image::paint(Panel *p)
{

}

void Image::doPaint(Panel *p)
{
	_panel = p;
	paint( p );
	_panel = nullptr;
}

Bitmap::Bitmap() :
	_rgba( nullptr ),
	_id( 0 ),
	_uploaded( false ) {	}

void Bitmap::paint( Panel *p )
{
	if( !_rgba )
		return;

	int wide, tall;
	getSize( wide, tall );

	if( !_id )
	{
		if( p->getSurfaceBase() )
			_id = p->getSurfaceBase()->createNewTextureID();
	}

	if( !_uploaded )
	{
		drawSetTextureRGBA( _id, (const char *)_rgba, wide, tall );
		_uploaded = true;
	}

	Color c;
	getColor( c );

	int r, g, b, a;
	c.getColor( r, g, b, a );

	drawSetTexture( _id );
	drawSetColor( r, g, b, a );

	int x, y;
	getPos( x, y );

	drawTexturedRect( x, y, x + wide, y + tall );
}

void Bitmap::setSize(int wide, int tall)
{
	Image::setSize( wide, tall );
	if( _rgba )
		delete[] _rgba;
	_rgba = new unsigned char[wide * tall * 4];
}

void Bitmap::setRGBA(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if( !_rgba )
		return;

	int wide, tall;
	getSize( wide, tall );

	if( x >= 0 && x < wide && y >= 0 && y < tall )
	{
		_rgba[( y * wide + x ) * 4 + 0] = r;
		_rgba[( y * wide + x ) * 4 + 1] = g;
		_rgba[( y * wide + x ) * 4 + 2] = b;
		_rgba[( y * wide + x ) * 4 + 3] = a;
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

	if( !_rgba )
		return false;

	if( hdr.m_IDLength != 0 )
		dis.seekRelative( hdr.m_IDLength, success );

	if( hdr.m_ImageType == 2 )
	{
		for( int y = tall - 1; y >= 0; y-- )
		{
			unsigned char *ptr = &_rgba[y * wide * 4];

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
			unsigned char *ptr = &_rgba[y * wide * 4];

			for( int x = 0; x < wide; x++ )
			{
				unsigned char pkthdr, pktsize;

				DIS_READ( pkthdr, readUChar );
				pktsize = ( pkthdr & 0x7f ) + 1;

				if( FBitSet( pkthdr, BIT( 7 )))
				{
					unsigned char color[4];

					switch( hdr.m_PixelDepth )
					{
					case 24:
						DIS_READ( color[2], readUChar );
						DIS_READ( color[1], readUChar );
						DIS_READ( color[0], readUChar );
						color[3] = invertAlpha ? 0 : 255;
						break;
					case 32:
						DIS_READ( color[2], readUChar );
						DIS_READ( color[1], readUChar );
						DIS_READ( color[0], readUChar );
						DIS_READ( color[3], readUChar );
						if( !invertAlpha )
							ptr[3] = 255 - ptr[3];
						break;
					}

					for( int j = 0; j < pktsize; j++ )
					{
						y++;
						memcpy( ptr, color, sizeof( color ));
						ptr += 4;
						if( y == wide )
						{
							if( x == 0 )
								goto quickexit;

							x--;
							ptr = &_rgba[y * x * 4];
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
							ptr = &_rgba[x * y * 4];
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

	if( _text == nullptr )
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
		int ch = _text[i];

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
			int ch2 = _text[i + 1];

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
			int ch2 = _text[i + word];

			if( ch2 == '\0' || ch2 == '\n' || ch2 == '\r' || ch == ' ' )
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
			ch = _text[i + j];

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
	// _size[0] = w;
	// _size[1] = h;
}

void TextImage::init(int len, const char *str)
{
	_schemeFont = Scheme::SF_PRIMARY1;
	_text = nullptr;
	_font = nullptr;
	_textBufferLen = 0;
	setText( str ); // ???

	int w, h;
	getTextSize( w, h );
	setSize( w, h );
}

void TextImage::getTextSize( int &w, int &h )
{
	w = 0;
	h = 0;

	if( _text == nullptr )
		return;

	Font *f = getFont();
	if( f == nullptr )
		return;

	f->getTextSize( _text, w, h );
}

void TextImage::getTextSizeWrapped( int &_w, int &_h )
{
	// a1ba: maybe not accurate, based on paint() code

	_w = 0;
	_h = 0;

	if( _text == nullptr )
		return;

	int w, h;
	getSize( w, h );

	Font *f = getFont();
	int x = 0, y = 0, tall = f->getTall();

	for( int i = 0; ; i++ )
	{
		int ch = _text[i];

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
			int ch2 = _text[i + 1];

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
			int ch2 = _text[i + word];

			if( ch2 == '\0' || ch2 == '\n' || ch2 == '\r' || ch == ' ' )
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
			ch = _text[i + j];

			f->getCharABCwide( ch, a, b, c );

			if( x + a + b + c > _w )
				_w = x + a + b + c;

			if( y + tall > _h )
				_h = y + tall;

			x += a + b + c;
		}

		i += word - 1;
	}
}

Font *TextImage::getFont()
{
	if( _font )
		return _font;

	return App::getInstance()->getScheme()->getFont( _schemeFont );
}

void TextImage::setText( int len, const char *str )
{
	if( len > _textBufferLen )
	{
		delete[] _text;
		_textBufferLen = len;
		_text = new char[len];

		if( !_text )
			_textBufferLen = 0;
	}

	if( _text )
		vgui_strcpy( _text, _textBufferLen, str );

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
	_schemeFont = sf;
}

void TextImage::setFont(Font *f)
{
	_font = f;
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
	_color.getColor( r, g, b, a );
	drawSetColor( r, g, b, a );

	drawFilledRect( 0, 0, w, _inset[1] );
	drawFilledRect( 0, h - _inset[3], w, h );
	drawFilledRect( 0, _inset[1], _inset[0], h - _inset[3] );
	drawFilledRect( w - _inset[2], _inset[1], w, h - _inset[3] );
}

void LineBorder::init(int thickness, Color c)
{
	setInset( thickness, thickness, thickness, thickness );
	_color = c;
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
	_panel = nullptr;
	_inset[0] = left;
	_inset[1] = top;
	_inset[2] = right;
	_inset[3] = bottom;
}

void Border::setInset(int left, int top, int right, int bottom)
{
	_inset[0] = left;
	_inset[1] = top;
	_inset[2] = right;
	_inset[3] = bottom;
}

void Border::getInset(int &left, int &top, int &right, int &bottom)
{
	left = _inset[0];
	top = _inset[1];
	right = _inset[2];
	bottom = _inset[3];
}

void Border::drawFilledRect(int x0, int y0, int x1, int y1)
{
	Image::drawFilledRect( x0 - _inset[0], y0 - _inset[1], x1 - _inset[0], y1 - _inset[1] );
}

void Border::drawOutlinedRect(int x0, int y0, int x1, int y1)
{
	Image::drawOutlinedRect( x0 - _inset[0], y0 - _inset[1], x1 - _inset[0], y1 - _inset[1] );
}

void Border::drawSetTextPos(int x, int y)
{
	Image::drawSetTextPos( x - _inset[0], y - _inset[1] );
}

void Border::drawPrintText(int x, int y, const char *str, int len)
{
	Image::drawPrintText( x - _inset[0], y - _inset[1], str, len );
}

void Border::drawPrintChar(int x, int y, char ch)
{
	Image::drawPrintChar( x - _inset[0], y - _inset[1], ch );
}

BorderPair::BorderPair(Border *border1, Border *border2)
{
	_border[0] = border1;
	_border[1] = border2;
}

void BorderPair::doPaint(Panel *p)
{
	if( _border[0] )
		_border[0]->doPaint( p );
	if( _border[1] )
		_border[1]->doPaint( p );
}

void BorderPair::paint(Panel *p)
{
	if( _border[0] )
		_border[0]->paint( p );
	if( _border[1] )
		_border[1]->paint( p );
}
