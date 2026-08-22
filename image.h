// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_DRAW_H
#define VGUI_DRAW_H

#include "vgui.h"
#include "scheme.h"

namespace vgui
{
class Panel;
class InputStream;

class CLASSEXPORT Color
{
public:
	Color();
	Color( int r, int g, int b, int a );
	Color( Scheme::SchemeColor sc );

private:
	virtual void init();

public:
	virtual void setColor( int r, int g, int b, int a );
	virtual void setColor( Scheme::SchemeColor sc );
	virtual void getColor( int &r, int &g, int &b, int &a );
	virtual void getColor( Scheme::SchemeColor &sc );
	virtual int operator[]( int i );

private:
	unsigned char rgba[4];
	Scheme::SchemeColor schemeColor;
};
CHECK_STRUCT_SIZE( Color, 12, 16, 16 );

class CLASSEXPORT Image
{
public:
	Image();
	virtual void setPos( int x, int y );
	virtual void getPos( int &x, int &y );
	virtual void getSize( int &w, int &h );
	virtual void setColor( Color c );
	virtual void getColor( Color &c );
	virtual void setSize( int w, int h );
	virtual void drawSetColor( Scheme::SchemeColor sc );
	virtual void drawSetColor( int r, int g, int b, int a );
	virtual void drawFilledRect( int x0, int y0, int x1, int y1 );
	virtual void drawOutlinedRect( int x0, int y0, int x1, int y1 );
	virtual void drawSetTextFont( Scheme::SchemeFont sf );
	virtual void drawSetTextFont( Font *font );
	virtual void drawSetTextColor( Scheme::SchemeColor sc );
	virtual void drawSetTextColor( int r, int g, int b, int a );
	virtual void drawSetTextPos( int x, int y );
	virtual void drawPrintText( const char *str, int len );
	virtual void drawPrintText( int x, int y, const char *str, int len );
	virtual void drawPrintChar( char ch );
	virtual void drawPrintChar( int x, int y, char ch );
	virtual void drawSetTextureRGBA( int id, const char *rgba, int w, int h );
	virtual void drawSetTexture( int id );
	virtual void drawTexturedRect( int x0, int y0, int x1, int y1 );
	virtual void paint( Panel *p );
	virtual void doPaint( Panel *p );

private:
	int    origin[2];
	int    size[2];
	Panel* panel;
	Color  color;
};
CHECK_STRUCT_SIZE( Image, 36, 48, 48 );

class CLASSEXPORT Bitmap : public Image
{
public:
	Bitmap();

	virtual void paint( Panel* ) override;

private:
	int  id;
	bool uploaded;

protected:
	virtual void setSize( int wide, int tall ) override;
	virtual void setRGBA( int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a );

	unsigned char* rgbaData;
};
CHECK_STRUCT_SIZE( Bitmap, 48, 64, 64 );

class CLASSEXPORT BitmapTGA : public Bitmap
{
public:
	BitmapTGA( InputStream *is, bool invertAlpha );
private:
	virtual bool loadTGA( InputStream *is, bool invertAlpha );
};
CHECK_STRUCT_SIZE( BitmapTGA, 48, 64, 64 );

class CLASSEXPORT TextImage : public Image
{
public:
	TextImage( int len, const char *str );
	TextImage( const char *str );

protected:
	virtual void paint( Panel *p ) override;

private:
	virtual void init( int len, const char *str );

public:
	virtual void getTextSize( int &, int & );
	virtual void getTextSizeWrapped( int &, int & );
	virtual Font *getFont();
	virtual void setText( int, const char * );
	virtual void setText( const char *str );
	virtual void setFont( Scheme::SchemeFont sf );
	virtual void setFont( Font *f );
	virtual void setSize( int w, int h ) override;

	char *text;
	int textBufferSize;
	Scheme::SchemeFont schemeFont;
	Font *font;
	int textColor[4];
	Scheme::SchemeColor textSchemeColor;
};
CHECK_STRUCT_SIZE( TextImage, 72, 96, 96 );

class CLASSEXPORT Border : public Image
{
public:
	Border() : Border( 0, 0, 0, 0 ) {}
	Border( int left, int top, int right, int bottom );
	virtual void setInset( int left, int top, int right, int bottom );
	virtual void getInset( int &left, int &top, int &right, int &bottom );

protected:
	virtual void drawFilledRect( int x0, int y0, int x1, int y1 ) override;
	virtual void drawOutlinedRect( int x0, int y0, int x1, int y1 ) override;
	virtual void drawSetTextPos( int x, int y ) override;
	virtual void drawPrintText( int x, int y, const char *str, int len ) override;
	virtual void drawPrintChar( int x, int y, char ch ) override;

	int inset[4];

private:
	Panel *panel;
};
CHECK_STRUCT_SIZE( Border, 56, 72, 72 );

class CLASSEXPORT BorderPair : public Border
{
public:
	BorderPair( Border *border1, Border *border2 );
	virtual void doPaint( Panel *p ) override;

protected:
	virtual void paint( Panel *p ) override;

	Border *borders[2];
};
CHECK_STRUCT_SIZE( BorderPair, 64, 88, 88 );

class CLASSEXPORT EtchedBorder : public Border
{
public:
	EtchedBorder();
protected:
	virtual void paint( Panel *p ) override;
};
CHECK_STRUCT_SIZE( EtchedBorder, 56, 72, 72 );

class CLASSEXPORT LineBorder : public Border
{
public:
	LineBorder( int thickness, Color c );
	LineBorder( int thickness );
	LineBorder( Color c );
	LineBorder();

protected:
	virtual void paint( Panel *p ) override;

private:
	virtual void init( int thickness, Color c );

	Color color;
};
CHECK_STRUCT_SIZE( LineBorder, 68, 88, 88 );

class CLASSEXPORT LoweredBorder : public Border
{
public:
	LoweredBorder();
protected:
	virtual void paint( Panel *p ) override;
};
CHECK_STRUCT_SIZE( LoweredBorder, 56, 72, 72 );

class CLASSEXPORT RaisedBorder : public Border
{
public:
	RaisedBorder();
protected:
	virtual void paint( Panel *p ) override;
};
CHECK_STRUCT_SIZE( RaisedBorder, 56, 72, 72 );
}

#endif // VGUI_DRAW_H
