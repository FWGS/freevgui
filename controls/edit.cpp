// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <stdarg.h>
#include "vgui_internal.h"
#include <stdio.h>

#include "controls/edit.h"
#include "app.h"
#include "scheme.h"
#include "font.h"
#include "signals.h"

using namespace vgui;

class DefaultEditPanelSignal : public InputSignalAdapter
{
	EditPanel *ep;
public:
	DefaultEditPanelSignal( EditPanel *ep ) : ep( ep ) {}

	virtual void mousePressed( MouseCode, Panel * ) override
	{
		ep->requestFocus();
		ep->repaint();
	}

	virtual void keyTyped( KeyCode code, Panel *p ) override
	{
		bool shift = p->isKeyDown( KEY_LSHIFT ) || p->isKeyDown( KEY_RSHIFT );

		switch( code )
		{
		case KEY_UP:
			ep->doCursorUp();
			break;
		case KEY_DOWN:
			ep->doCursorDown();
			break;
		case KEY_LEFT:
			ep->doCursorLeft();
			break;
		case KEY_RIGHT:
			ep->doCursorRight();
			break;
		case KEY_HOME:
			ep->doCursorToStartOfLine();
			break;
		case KEY_END:
			ep->doCursorToEndOfLine();
			break;
		case KEY_BACKSPACE:
			ep->doCursorBackspace();
			break;
		case KEY_DELETE:
			ep->doCursorDelete();
			break;
		case KEY_ENTER:
			ep->doCursorNewLine();
			break;
		default:
		{
			char ch = ep->getApp()->getKeyCodeChar( code, shift );
			if( ch != 0 )
				ep->doCursorInsertChar( ch );
			break;
		}
		}
	}

	virtual void keyFocusTicked( Panel * ) override
	{
		bool blink;
		int next;

		ep->getCursorBlink( blink, next );
		if( ep->getApp()->getTimeMillis() > next )
			ep->setCursorBlink( !blink );
	}
};

EditPanel::EditPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall ), cursorPos{ 0, 0 }, font( nullptr )
{
	setCursorBlink( true );
	addInputSignal( new DefaultEditPanelSignal( this ));
	getLine( 0 );
}

void EditPanel::doCursorUp()
{
	if( cursorPos[1] > 0 )
	{
		Dar<char> *src = getLine( cursorPos[1] );
		Dar<char> *dst = getLine( cursorPos[1] - 1 );

		cursorPos[0] = spatialCharOffsetBetweenTwoLines( src, dst, cursorPos[0] );
		cursorPos[1]--;
	}

	setCursorBlink( true );
}

void EditPanel::doCursorDown()
{
	int visibleLines = getVisibleLineCount();
	int count = getLineCount();
	int maxLine = visibleLines < count ? visibleLines : count;

	if( cursorPos[1] + 1 < maxLine )
	{
		Dar<char> *src = getLine( cursorPos[1] );
		Dar<char> *dst = getLine( cursorPos[1] + 1 );

		cursorPos[0] = spatialCharOffsetBetweenTwoLines( src, dst, cursorPos[0] );
		cursorPos[1]++;
	}

	setCursorBlink( true );
}

void EditPanel::doCursorLeft()
{
	if( cursorPos[0] > 0 )
		cursorPos[0]--;

	setCursorBlink( true );
}

void EditPanel::doCursorRight()
{
	cursorPos[0]++;
	setCursorBlink( true );
}

void EditPanel::doCursorToStartOfLine()
{
	cursorPos[0] = 0;
}

void EditPanel::doCursorToEndOfLine()
{
	Dar<char> *line = getLine( cursorPos[1] );

	if( !line )
		return;

	cursorPos[0] = line->getCount();
}

void EditPanel::doCursorInsertChar( char ch )
{
	Dar<char> *line = getLine( cursorPos[1] );

	if( !line )
		return;

	shiftLineRight( line, cursorPos[0], 1 );
	setChar( line, cursorPos[0], ch );
	doCursorRight();
	repaint();
}

void EditPanel::doCursorBackspace()
{
	Dar<char> *line = getLine( cursorPos[1] );

	if( !line )
		return;

	if( cursorPos[0] == 0 )
	{
		Dar<char> *prev = getLine( cursorPos[1] - 1 );

		if( !prev )
			return;

		int prevLen = prev->getCount();

		for( int i = 0; i < line->getCount(); i++ )
			prev->addElement( (*line)[i] );

		lines.removeElementAt( cursorPos[1] );
		cursorPos[1]--;
		cursorPos[0] = prevLen;
	}
	else
	{
		shiftLineLeft( line, cursorPos[0], 1 );
		doCursorLeft();
	}

	repaint();
}

void EditPanel::doCursorNewLine()
{
	Dar<char> *line = getLine( cursorPos[1] );

	if( !line )
		return;

	Dar<char> *newLine = new Dar<char>();

	for( int i = cursorPos[0]; i < line->getCount(); i++ )
		newLine->addElement( (*line)[i] );

	lines.insertElementAt( newLine, cursorPos[1] + 1 );
	line->setCount( cursorPos[0] );
	cursorPos[0] = 0;
	doCursorDown();
	repaint();
}

void EditPanel::doCursorDelete()
{
	doCursorRight();
	shiftLineLeft( getLine( cursorPos[1] ), cursorPos[0], 1 );
	doCursorLeft();
	repaint();
}

void EditPanel::doCursorPrintf( char *format, ... )
{
	char buf[8192];
	va_list va;

	va_start( va, format );
	vsprintf( buf, format, va );
	va_end( va );

	for( char *p = buf; *p; p++ )
	{
		if( *p == '\n' )
			doCursorNewLine();
		else
			doCursorInsertChar( *p );
	}

	repaint();
}

int EditPanel::getLineCount()
{
	return lines.getCount();
}

int EditPanel::getVisibleLineCount()
{
	int w, h;

	getPaintSize( w, h );

	Font *f = font ? font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );

	return h / f->getTall();
}

void EditPanel::setCursorBlink( bool state )
{
	cursorBlink = state;
	cursorNextBlinkTime = (int)( getApp()->getTimeMillis() + 400 );
	repaint();
}

void EditPanel::setFont( Font *f )
{
	font = f;
	repaint();
}

void EditPanel::getText( int lineIndex, int offset, char *buf, int bufLen )
{
	Dar<char> *line = getLine( lineIndex );

	if( !line )
		return;

	int i, j;

	for( i = offset, j = 0; i < line->getCount() && j < bufLen - 1; i++, j++ )
		buf[i - offset] = (*line)[i];

	buf[j] = '\0';
}

void EditPanel::getCursorBlink( bool &blink, int &nextBlinkTime )
{
	blink = cursorBlink;
	nextBlinkTime = cursorNextBlinkTime;
}

void EditPanel::paintBackground()
{
	int w, h;

	getPaintSize( w, h );
	drawSetColor( Scheme::SC_WHITE );
	drawFilledRect( 0, 0, w, h );
}

void EditPanel::paint()
{
	Font *f = font ? font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );
	int lineHeight = f->getTall();
	int y = 0;

	drawSetTextFont( f );

	for( int line = 0; line < lines.getCount(); line++ )
	{
		Dar<char> *lineDar = lines[line];
		int x = 0;
		int caretX = 0;

		drawSetTextColor( Scheme::SC_BLACK );

		for( int col = 0; col < lineDar->getCount(); col++ )
		{
			char ch = (*lineDar)[col];
			int a, b, c;

			if( line == cursorPos[1] && col == cursorPos[0] )
				caretX = x;

			f->getCharABCwide( ch, a, b, c );
			drawPrintChar( x, y, ch );
			x += a + b + c;
		}

		if( line == cursorPos[1] && cursorPos[0] >= lineDar->getCount() )
		{
			int a, b, c;

			f->getCharABCwide( ' ', a, b, c );
			caretX = x + ( cursorPos[0] - lineDar->getCount() ) * ( a + b + c );
		}

		if( line == cursorPos[1] && cursorBlink )
		{
			drawSetColor( 255, 0, 0, 0 );
			drawFilledRect( caretX - 1, y, caretX + 1, y + lineHeight );
		}

		y += lineHeight;
	}
}

void EditPanel::addLine()
{
}

Dar<char> *EditPanel::getLine( int lineIndex )
{
	if( lineIndex < 0 )
		return nullptr;

	if( lineIndex == 0 && lines.getCount() == 0 )
	{
		Dar<char> *line = new Dar<char>();

		lines.addElement( line );
		return line;
	}

	if( lineIndex >= lines.getCount() )
		return nullptr;

	return lines[lineIndex];
}

void EditPanel::setChar( Dar<char> *lineDar, int x, char ch, char fill )
{
	if( !lineDar || x < 0 )
		return;

	while( lineDar->getCount() < x )
		lineDar->addElement( fill );

	if( x < lineDar->getCount() )
		lineDar->setElementAt( ch, x );
	else
		lineDar->addElement( ch );
}

void EditPanel::setChar( Dar<char> *lineDar, int x, char ch )
{
	setChar( lineDar, x, ch, ' ' );
}

void EditPanel::shiftLineLeft( Dar<char> *lineDar, int x, int count )
{
	if( !lineDar || count < 0 || x < count )
		return;

	int n = lineDar->getCount();

	if( x > n )
		return;

	for( int i = x; i < n; i++ )
		lineDar->setElementAt( (*lineDar)[i], i - count );

	lineDar->setCount( n - count );
}

void EditPanel::shiftLineRight( Dar<char> *lineDar, int x, int count )
{
	if( !lineDar || x < 0 || count < 0 )
		return;

	int n = lineDar->getCount();

	for( int i = 0; i < count; i++ )
		lineDar->addElement( ' ' );

	for( int i = n - 1; i >= x; i-- )
		lineDar->setElementAt( (*lineDar)[i], i + count );
}

int EditPanel::spatialCharOffsetBetweenTwoLines( Dar<char> *src, Dar<char> *dst, int x )
{
	if( !src || !dst )
		return x;

	Font *f = font ? font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );

	int pixelX = 0;

	for( int i = 0; i < x && i < src->getCount(); i++ )
	{
		int a, b, c;

		f->getCharABCwide( (*src)[i], a, b, c );
		pixelX += a + b + c;
	}

	int col = 0, cur = 0;

	while( col < dst->getCount() && cur < pixelX )
	{
		int a, b, c;

		f->getCharABCwide( (*dst)[col], a, b, c );
		cur += a + b + c;
		col++;
	}

	return col;
}
