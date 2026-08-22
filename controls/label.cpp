// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "label.h"
#include "vgui_internal.h"
#include "treefolder.h"
#include "text.h"
#include "image.h"
#include "layout.h"

namespace vgui
{
class LabelProperties : public Panel, public ActionSignal
{
public:
	Label *editedLabel;
	TextEntry *textEntry;

	LabelProperties( Label *label ) : Panel( 0, 0, 200, 30 ), editedLabel( label ), textEntry( new TextEntry( "", 0, 0, 80, 20 ))
	{
		setLayout( new FlowLayout( 2 ));

		Label *l = new Label( "setText" );
		l->setParent( this );

		textEntry->setParent( this );
		textEntry->addActionSignal( this );
	}

	void actionPerformed( Panel *p )
	{
		char buf[256];

		textEntry->getText( 0, buf, sizeof( buf ));
		editedLabel->setText( buf );
		editedLabel->repaint();
	}
};

Label::Label( const char *str ) :
	Label( str, 0, 0 )
{

}

Label::Label( const char *str, int x, int y ) :
	Panel( x, y, 10, 10 )
{
	init( strlen( str ) + 1, str, true );
}

Label::Label( const char *str, int x, int y, int w, int h ) :
	Label( strlen( str ) + 1, str, x, y, w, h )
{

}

Label::Label( int len, const char *str, int x, int y, int w, int h ) :
	Panel( x, y, w, h )
{
	init( len, str, false );
}

void Label::setImage( Image *newImage )
{
	image = newImage;
	recomputeMinimumSize();
	if( image )
		repaint();
}

void Label::setText( int len, const char *str )
{
	textImage->setText( len, str );
	recomputeMinimumSize();
	repaint();
}

void Label::setText( const char *str, ... )
{
	char buf[4096];
	va_list va;

	if( !str )
	{
		setText( 6, "NULL!" );
		return;
	}

	va_start( va, str );
	int len = vsnprintf( buf, sizeof( buf ), str, va );
	va_end( va );

	if( len < 0 )
	{
		setText( 10, "OVERFLOW!" );
		return;
	}

	setText( len + 1, buf );
}

void Label::setFont( Scheme::SchemeFont sf )
{
	textImage->setFont( sf );
	recomputeMinimumSize();
	repaint();
}

void Label::setFont( Font *f )
{
	textImage->setFont( f );
	recomputeMinimumSize();
	repaint();
}

void Label::getTextSize( int &w, int &h )
{
	textImage->getSize( w, h );
}

void Label::getContentSize( int &w, int &h )
{
	int tx0, ty0, tx1, ty1;
	int ix0, iy0, ix1, iy1;
	int minX, minY, maxX, maxY;

	computeAlignment( tx0, ty0, tx1, ty1, ix0, iy0, ix1, iy1, minX, minY, maxX, maxY );

	w = maxX - minX;
	h = maxY - minY;
}

void Label::setTextAlignment( Alignment alignment )
{
	textAlignment = alignment;
	recomputeMinimumSize();
	repaint();
}

void Label::setContentAlignment( Alignment alignment )
{
	contentAlignment = alignment;
	recomputeMinimumSize();
	repaint();
}

Panel *Label::createPropertyPanel()
{
	Panel *p = Panel::createPropertyPanel();

	TreeFolder *tf = new TreeFolder( "Label" );
	p->addChild( tf );
	tf->addChild( new LabelProperties( this ));
	tf->addChild( new Label( "setContentAlignment" ));

	return p;
}

void Label::setFgColor( int r, int g, int b, int a )
{
	Panel::setFgColor( r, g, b, a );
	textImage->setColor( Color( r, g, b, a	) );
	repaint();
}

void Label::setFgColor( Scheme::SchemeColor scheme_color )
{
	Panel::setFgColor( scheme_color );
	textImage->setColor( scheme_color );
	repaint();
}

void Label::setContentFitted( bool fit )
{
	contentFitted = fit;
	recomputeMinimumSize();
	repaint();
}

void Label::recomputeMinimumSize()
{
	int w, h;
	getContentSize( w, h );
	setPreferredSize( w, h );
	if( contentFitted )
		setSize( w, h );
}

void Label::computeAlignment( int &tx0, int &ty0, int &tx1, int &ty1, int &ix0, int &iy0, int &ix1, int &iy1, int &minX, int &minY, int &maxX, int &maxY )
{
	int w, h;
	getPaintSize( w, h );

	int tw, th;
	getTextSize( tw, th );

	ix0 = iy0 = ix1 = iy1 = 0;
	if( image )
		image->getSize( ix1, iy1 );

	tx0 = 0;
	ty0 = 0;
	tx1 = tw;
	ty1 = th;

	switch( textAlignment )
	{
	case TOPLEFT:
	case LEFT:
	case BOTTOMLEFT:
		tx0 = ix0 - tw;
		break;
	case TOP:
	case CENTER:
	case BOTTOM:
		tx0 = ( ix1 - ix0 ) / 2 - ( tx1 - tx0 ) / 2;
		break;
	case TOPRIGHT:
	case RIGHT:
	case BOTTOMRIGHT:
		tx0 = ix1;
		break;
	}

	switch( textAlignment )
	{
	case TOPLEFT:
	case TOP:
	case TOPRIGHT:
		ty0 = iy0 - th;
		break;
	case LEFT:
	case CENTER:
	case RIGHT:
		ty0 = ( iy1 - iy0 ) / 2 - ( ty1 - ty0 ) / 2;
		break;
	case BOTTOMLEFT:
	case BOTTOM:
	case BOTTOMRIGHT:
		ty0 = iy1;
		break;
	}

	tx1 = tx0 + tw;
	ty1 = ty0 + th;

	minX = Q_min( tx0, ix0 );
	minY = Q_min( ty0, iy0 );

	maxX = Q_max( tx1, ix1 );
	maxY = Q_max( ty1, iy1 );

	tx0 -= minX; ty0 -= minY; tx1 -= minX; ty1 -= minY;
	ix0 -= minX; iy0 -= minY; ix1 -= minX; iy1 -= minY;

	maxX -= minX;
	maxY -= minY;
	minX = 0;
	minY = 0;

	int offx, offy;
	switch( contentAlignment )
	{
	case TOPLEFT:
	case LEFT:
	case BOTTOMLEFT:
		offx = 0;
		break;
	case TOP:
	case CENTER:
	case BOTTOM:
		offx = w / 2 - ( maxX - minX ) / 2;
		break;
	case TOPRIGHT:
	case RIGHT:
	case BOTTOMRIGHT:
		offx = w - ( maxX - minX );
		break;
	}

	switch( contentAlignment )
	{
	case TOPLEFT:
	case TOP:
	case TOPRIGHT:
		offy = 0;
		break;
	case LEFT:
	case CENTER:
	case RIGHT:
		offy = h / 2 - maxY / 2;
		break;
	case BOTTOMLEFT:
	case BOTTOM:
	case BOTTOMRIGHT:
		offy = h - maxY;
		break;
	}

	tx0 += offx; ty0 += offy; tx1 += offx; ty1 += offy;
	ix0 += offx; iy0 += offy; ix1 += offx; iy1 += offy;

	minX += offx - 4; minY += offy - 4; maxX += offx + 4; maxY += offy + 4;
}

void Label::paint()
{
	int tx0, ty0, tx1, ty1;
	int ix0, iy0, ix1, iy1;
	int minX, minY, maxX, maxY;

	computeAlignment( tx0, ty0, tx1, ty1, ix0, iy0, ix1, iy1, minX, minY, maxX, maxY );

	if( image )
	{
		image->setPos( ix0, iy0 );
		image->doPaint( this );
	}

	if( textImage )
	{
		textImage->setPos( tx0, ty0 );
		textImage->doPaint( this );
	}

	if( hasFocus() )
	{
		drawSetColor( Scheme::SC_PRIMARY2 );
		drawOutlinedRect( tx0, ty0, tx1, ty1 );
	}
}

void Label::init( int len, const char *str, bool textFitted )
{
	contentFitted = textFitted;
	textAlignment = CENTER;
	contentAlignment = CENTER;
	textImage = new TextImage( len, str );
	textImage->setColor( Color( Scheme::SC_BLACK ));
	image = nullptr;
	setText( len, str );
}

IntLabel::IntLabel( int value, int x, int y, int wide, int tall ) :
	Label( nullptr, x, y, wide, tall ), value( value )
{
}

void IntLabel::setValue( int newValue )
{
	if( value == newValue )
		return;

	value = newValue;
	repaint();
}

void IntLabel::intChanged( int value, Panel *p )
{
	setValue( value );
}

void IntLabel::paintBackground()
{
	char buf[50];

	snprintf( buf, sizeof( buf ), "%d", value );
	Panel::paintBackground();
	drawSetTextFont( Scheme::SF_PRIMARY1 );
	drawSetTextColor( Scheme::SC_BLACK );
	drawPrintText( 0, 0, buf, strlen( buf ));
}

}
