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

class FooDefaultEditPanelSignal : public InputSignalAdapter
{
	EditPanel *ep;
public:
	FooDefaultEditPanelSignal( EditPanel *ep ) : ep( ep ) {}

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

EditPanel::EditPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall ), _cursor{ 0, 0 }, _font( nullptr )
{
	setCursorBlink( true );
	addInputSignal( new FooDefaultEditPanelSignal( this ));
	getLine( 0 );
}

void EditPanel::doCursorUp()
{
	if( _cursor[1] > 0 )
	{
		Dar<char> *src = getLine( _cursor[1] );
		Dar<char> *dst = getLine( _cursor[1] - 1 );

		_cursor[0] = spatialCharOffsetBetweenTwoLines( src, dst, _cursor[0] );
		_cursor[1]--;
	}

	setCursorBlink( true );
}

void EditPanel::doCursorDown()
{
	int visible = getVisibleLineCount();
	int count = getLineCount();
	int maxLine = visible < count ? visible : count;

	if( _cursor[1] + 1 < maxLine )
	{
		Dar<char> *src = getLine( _cursor[1] );
		Dar<char> *dst = getLine( _cursor[1] + 1 );

		_cursor[0] = spatialCharOffsetBetweenTwoLines( src, dst, _cursor[0] );
		_cursor[1]++;
	}

	setCursorBlink( true );
}

void EditPanel::doCursorLeft()
{
	if( _cursor[0] > 0 )
		_cursor[0]--;

	setCursorBlink( true );
}

void EditPanel::doCursorRight()
{
	_cursor[0]++;
	setCursorBlink( true );
}

void EditPanel::doCursorToStartOfLine()
{
	_cursor[0] = 0;
}

void EditPanel::doCursorToEndOfLine()
{
	Dar<char> *line = getLine( _cursor[1] );

	if( !line )
		return;

	_cursor[0] = line->getCount();
}

void EditPanel::doCursorInsertChar( char ch )
{
	Dar<char> *line = getLine( _cursor[1] );

	if( !line )
		return;

	shiftLineRight( line, _cursor[0], 1 );
	setChar( line, _cursor[0], ch );
	doCursorRight();
	repaint();
}

void EditPanel::doCursorBackspace()
{
	Dar<char> *line = getLine( _cursor[1] );

	if( !line )
		return;

	if( _cursor[0] == 0 )
	{
		Dar<char> *prev = getLine( _cursor[1] - 1 );

		if( !prev )
			return;

		int prevLen = prev->getCount();

		for( int i = 0; i < line->getCount(); i++ )
			prev->addElement( (*line)[i] );

		_lineDarDar.removeElementAt( _cursor[1] );
		_cursor[1]--;
		_cursor[0] = prevLen;
	}
	else
	{
		shiftLineLeft( line, _cursor[0], 1 );
		doCursorLeft();
	}

	repaint();
}

void EditPanel::doCursorNewLine()
{
	Dar<char> *line = getLine( _cursor[1] );

	if( !line )
		return;

	Dar<char> *newLine = new Dar<char>();

	for( int i = _cursor[0]; i < line->getCount(); i++ )
		newLine->addElement( (*line)[i] );

	_lineDarDar.insertElementAt( newLine, _cursor[1] + 1 );
	line->setCount( _cursor[0] );
	_cursor[0] = 0;
	doCursorDown();
	repaint();
}

void EditPanel::doCursorDelete()
{
	doCursorRight();
	shiftLineLeft( getLine( _cursor[1] ), _cursor[0], 1 );
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
	return _lineDarDar.getCount();
}

int EditPanel::getVisibleLineCount()
{
	int w, h;

	getPaintSize( w, h );

	Font *font = _font ? _font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );

	return h / font->getTall();
}

void EditPanel::setCursorBlink( bool state )
{
	_cursorBlink = state;
	_cursorNextBlinkTime = (int)( getApp()->getTimeMillis() + 400 );
	repaint();
}

void EditPanel::setFont( Font *font )
{
	_font = font;
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
	blink = _cursorBlink;
	nextBlinkTime = _cursorNextBlinkTime;
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
	Font *font = _font ? _font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );
	int lineHeight = font->getTall();
	int y = 0;

	drawSetTextFont( font );

	for( int line = 0; line < _lineDarDar.getCount(); line++ )
	{
		Dar<char> *lineDar = _lineDarDar[line];
		int x = 0;
		int caretX = 0;

		drawSetTextColor( Scheme::SC_BLACK );

		for( int col = 0; col < lineDar->getCount(); col++ )
		{
			char ch = (*lineDar)[col];
			int a, b, c;

			if( line == _cursor[1] && col == _cursor[0] )
				caretX = x;

			font->getCharABCwide( ch, a, b, c );
			drawPrintChar( x, y, ch );
			x += a + b + c;
		}

		if( line == _cursor[1] && _cursor[0] >= lineDar->getCount() )
		{
			int a, b, c;

			font->getCharABCwide( ' ', a, b, c );
			caretX = x + ( _cursor[0] - lineDar->getCount() ) * ( a + b + c );
		}

		if( line == _cursor[1] && _cursorBlink )
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

	if( lineIndex == 0 && _lineDarDar.getCount() == 0 )
	{
		Dar<char> *line = new Dar<char>();

		_lineDarDar.addElement( line );
		return line;
	}

	if( lineIndex >= _lineDarDar.getCount() )
		return nullptr;

	return _lineDarDar[lineIndex];
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

	Font *font = _font ? _font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );

	int pixelX = 0;

	for( int i = 0; i < x && i < src->getCount(); i++ )
	{
		int a, b, c;

		font->getCharABCwide( (*src)[i], a, b, c );
		pixelX += a + b + c;
	}

	int col = 0, cur = 0;

	while( col < dst->getCount() && cur < pixelX )
	{
		int a, b, c;

		font->getCharABCwide( (*dst)[col], a, b, c );
		cur += a + b + c;
		col++;
	}

	return col;
}
