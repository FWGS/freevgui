// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <stdarg.h>
#include "vgui_internal.h"
#include <stdio.h>
#include "app.h"
#include "text.h"
#include "font.h"

namespace vgui
{
TextEntry::TextEntry( const char *str, int x, int y, int w, int h ) : Panel( x, y, w, h ),
	cursorPos( 0 ), hideText( false ), cursorBlinkRate( 400 ), font( nullptr )
{
	selection[0] = -1;
	selection[1] = -1;
	resetCursorBlink();
	setText( str, strlen( str ));
	addInputSignal( this );
	addFocusChangeSignal( makeFocusChangeHandler([this]( bool, Panel * )
	{
		resetCursorBlink();
		doSelectNone();
	}));
}

void TextEntry::setText( const char *text, int len )
{
	line.removeAll();
	line.ensureCapacity( len );

	for( int i = 0; i < len; i++ )
	{
		line.addElement( text[i] );
		setCharAt( text[i], i );
	}

	doGotoEndOfLine();
}

void TextEntry::getText( int off, char *buf, int len )
{
	if( !buf )
		return;

	for( int i = off; i < len - 1; i++ )
	{
		buf[i - off] = 0;
		if( i >= line.getCount())
			break;

		buf[i - off] = line[i];
	}
	buf[len - 1] = 0;
}

void TextEntry::resetCursorBlink()
{
	cursorBlink = false;
	cursorNextBlinkTime = getApp()->getTimeMillis() + cursorBlinkRate;
}

void TextEntry::doGotoLeft()
{
	selectCheck();
	cursorPos = Q_max( cursorPos - 1, 0 );
	resetCursorBlink();
	repaint();
}

void TextEntry::doGotoRight()
{
	selectCheck();
	cursorPos = Q_min( cursorPos + 1, line.getCount());
	resetCursorBlink();
	repaint();
}

void TextEntry::doGotoFirstOfLine()
{
	selectCheck();
	cursorPos = 0;
	resetCursorBlink();
	repaint();
}

void TextEntry::doGotoEndOfLine()
{
	selectCheck();
	cursorPos = line.getCount();
	resetCursorBlink();
	repaint();
}

void TextEntry::doInsertChar( char ch )
{
	line.ensureCapacity( line.getCount() + 1 );
	line.setCount( line.getCount() + 1 );
	for( int i = line.getCount() - 1; i >= cursorPos; i-- )
		setCharAt( line[i], i + 1 );

	setCharAt( ch, cursorPos );
	cursorPos++;
	resetCursorBlink();
	repaint();
}

void TextEntry::doBackspace()
{
	if( !cursorPos || !line.getCount( ))
		return;

	for( int i = cursorPos; i < line.getCount(); i++ )
		setCharAt( line[i],	i - 1 );

	line.setCount( line.getCount() - 1 );
	cursorPos--;
	resetCursorBlink();
	repaint();
}

void TextEntry::doDelete()
{
	if( !line.getCount() || cursorPos != line.getCount( ))
		return;

	for( int i = cursorPos + 1; i < line.getCount(); i++ )
		setCharAt( line[i], i - 1 );

	line.setCount( line.getCount() - 1 );
	cursorPos--;
	resetCursorBlink();
	repaint();
}

void TextEntry::doSelectNone()
{
	selection[0] = -1;
	repaint();
}

void TextEntry::doCopySelected()
{
	int x, y;

	if( !getSelectedRange( x, y ))
		return;

	char buf[200];
	int i = 0;

	for( ; i < sizeof( buf ) - 1; i++ )
	{
		if( x + i >= y )
			break;

		buf[i] = line[x + i];
	}

	buf[i] = 0;
	getApp()->setClipboardText( buf, i );
}

void TextEntry::doPaste()
{
	char buf[256];
	int len = getApp()->getClipboardText( 0, buf, sizeof( buf ));

	for( int i = 0; i < len; i++ )
		doInsertChar( buf[i] );
}

void TextEntry::doPasteSelected()
{
	doDeleteSelected();
	doPaste();
}

void TextEntry::doDeleteSelected()
{
	// not implemented in original vgui
}

void TextEntry::addActionSignal( ActionSignal *as )
{
	actionSignals.putElement( as );
}

void TextEntry::setFont( Font *f )
{
	font = f;
}

void TextEntry::setTextHidden( bool hide )
{
	hideText = hide;
	repaint();
}

void TextEntry::paintBackground()
{
	Font *f = font ? font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );
	int text_h = f->getTall();

	{
		int x, y;
		if( getSelectedPixelRange( x, y ))
		{
			x += 3;
			y += 3;
			drawSetColor( Scheme::SC_WHITE );
			drawFilledRect( 0, 0, x, text_h + 1 );
			drawFilledRect( x, 0, size[0], text_h + 1 );
			drawSetColor( 0, 0, 200, 0 );
			drawFilledRect( x, 0, y, text_h + 1 );
		}
		else
		{
			drawSetColor(Scheme::SC_WHITE);
			drawFilledRect(0,0,size[0],size[1]);
		}
	}

	drawSetTextFont( f );
	drawSetColor( Scheme::SC_BLACK );
	drawSetTextPos( 3, 0 );

	for( int i = 0; i < line.getCount(); i++ )
	{
		if( hideText )
			drawPrintChar( '*' );
		else
			drawPrintChar( line[i] );
	}

	if( hasFocus( ))
	{
		drawSetColor( Scheme::SC_BLACK );
		drawFilledRect( 0, 0, size[0], 1 );
		drawFilledRect( 0, size[1] - 1, size[0], size[1] );
		drawFilledRect( 0, 1, 1, size[1] - 1 );
		drawFilledRect( size[0] - 1, 1, size[0], size[1] - 1 );

		if( !cursorBlink )
		{
				int x = cursorToPixelSpace( cursorPos );
				drawSetColor( Scheme::SC_BLACK );
				drawFilledRect( x + 3, 2, x + 4, text_h - 1 );
		}
	}
}

void TextEntry::setCharAt( char ch, int at )
{
	if( at < 0 )
		return;

	line.ensureCapacity( at + 1 );
	line.setElementAt( ch, at );
}

void TextEntry::fireActionSignal()
{
	for( auto signal : actionSignals )
		signal->actionPerformed( this );
}

bool TextEntry::getSelectedRange( int &x, int &y )
{
	if( selection[0] == -1 )
		return false;

	x = selection[0];
	y = selection[1];
	if( x > y )
	{
		int temp = x;
		x = y;
		y = temp;
	}

	return true;
}

bool TextEntry::getSelectedPixelRange( int &x, int &y )
{
	if( !getSelectedRange( x, y ))
		return false;

	x = cursorToPixelSpace( x );
	y = cursorToPixelSpace( y );
	return true;
}

int TextEntry::cursorToPixelSpace( int at )
{
	Font *f = font ? font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );

	int x = 0;
	for( int i = 0; i < line.getCount(); i++ )
	{
		if( i == at )
			break;

		int a, b, c;
		if( hideText )
			f->getCharABCwide( '*', a, b, c );
		else f->getCharABCwide( line[i], a, b, c );

		x += a + b + c;
	}

	return x;
}

void TextEntry::selectCheck()
{
	if( isKeyDown( KEY_LSHIFT ) || isKeyDown( KEY_RSHIFT ))
		selection[0] = cursorPos;
	else selection[0] = -1;
}

void TextEntry::cursorMoved( int, int, Panel * )
{

}

void TextEntry::cursorEntered( Panel* )
{

}

void TextEntry::cursorExited( Panel* )
{

}

void TextEntry::mousePressed( MouseCode, Panel* )
{
	resetCursorBlink();
	requestFocus();
	repaint();
}

void TextEntry::mouseDoublePressed( MouseCode, Panel* )
{

}

void TextEntry::mouseReleased( MouseCode, Panel* )
{

}

void TextEntry::mouseWheeled( int, Panel* )
{

}

void TextEntry::keyPressed( KeyCode, Panel* )
{

}

void TextEntry::keyTyped( KeyCode code, Panel *p )
{
	bool shift = p->isKeyDown( KEY_LSHIFT ) || p->isKeyDown( KEY_RSHIFT );
	bool ctrl = p->isKeyDown( KEY_LCONTROL ) || p->isKeyDown( KEY_RCONTROL );

	if( ctrl )
	{
		switch( code )
		{
		case KEY_C:
			doCopySelected();
			break;
		case KEY_V:
			doPaste();
			break;
		default:
			break;
		}
	}
	else
	{
		char ch;

		switch( code )
		{
		case KEY_INSERT:
			if( shift )
				doPaste();
			break;
		case KEY_DELETE:
			shift ? doDeleteSelected() : doDelete();
			break;
		case KEY_LEFT:
			doGotoLeft();
			break;
		case KEY_RIGHT:
			doGotoRight();
			break;
		case KEY_HOME:
			doGotoFirstOfLine();
			break;
		case KEY_END:
			doGotoEndOfLine();
			break;
		case KEY_BACKSPACE:
			doBackspace();
			break;
		case KEY_ENTER:
		case KEY_TAB:
		case KEY_LSHIFT:
		case KEY_RSHIFT:
			break;
		default:
			ch = getApp()->getKeyCodeChar( code, shift );
			if( ch != 0 )
				doInsertChar( ch );
			break;
		}
	}

	selection[1] = cursorPos;

	if( code == KEY_ENTER )
		fireActionSignal();
}

void TextEntry::keyReleased( KeyCode, Panel* )
{

}

void TextEntry::keyFocusTicked( Panel* )
{
	int time = getApp()->getTimeMillis();
	if( time > cursorNextBlinkTime )
	{
		cursorBlink = !cursorBlink;
		cursorNextBlinkTime = time + cursorBlinkRate;
		repaint();
	}
}

TextGrid::TextGrid( int grid_w, int grid_h, int x, int y, int w, int h ) : Panel( x, y, w, h ),
	grid( nullptr ), gridSize{ 0, 0 }, cursorPos{ 0, 0 }
{
	setGridSize( grid_w, grid_h );
	setBgColor( 255, 255, 255, 0 );
	setFgColor( 0, 0, 0, 0 );
}

void TextGrid::setGridSize( int w, int h )
{
	if( w <= 0 || h <= 0 )
		return;

	delete[] grid;
	grid = new char[w * h * 7];
	memset( grid, 0, w * h * 7 );
	gridSize[0] = w;
	gridSize[1] = h;
}

void TextGrid::newLine()
{
	if( cursorPos[1] == gridSize[1] - 1 )
	{
		if( gridSize[1] > 1 )
		{
			size_t lineSize = gridSize[0] * 7;

			memset( &grid[( cursorPos[1] * gridSize[0] + cursorPos[0] ) * 7], 0, gridSize[0] - cursorPos[0] );
			for( int j = 1; j < gridSize[1]; j++ )
			{
				memcpy( &grid[( j - 1 ) * lineSize], &grid[j * lineSize], lineSize );
			}
			memset( &grid[cursorPos[1] * lineSize], 0, lineSize );
		}

		cursorPos[0] = 0;
	}
	else
	{
		cursorPos[0] = 0;
		++cursorPos[1];
	}
}

void TextGrid::setXY( int x, int y )
{
	cursorPos[0] = x;
	cursorPos[1] = y;
}

int TextGrid::vprintf( const char *fmt, va_list va )
{
	char buf[2048];

	int ret = vsprintf( buf, fmt, va );

	for( int i = 0; i < sizeof( buf ); i++ )
	{
		if( !buf[i] )
			break;

		if( buf[i] == '\n' )
		{
			newLine();
			continue;
		}

		if( cursorPos[0] >= 0 && cursorPos[0] < gridSize[0] )
		{
			if( cursorPos[1] >= 0 && cursorPos[1] < gridSize[1] )
			{
				grid[(cursorPos[1] * gridSize[0] + cursorPos[0]) * 7] = buf[i];
				++cursorPos[0];
			}
		}
	}

	repaint();

	return ret;
}

int TextGrid::printf( const char *fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	int ret = vprintf( fmt, va );
	va_end( va );

	return ret;
}

void TextGrid::paintBackground()
{
	Panel::paintBackground();
	Font *font = getApp()->getScheme()->getFont( Scheme::SF_PRIMARY2 );

	int a, b, c;
	font->getCharABCwide( 'W', a, b, c );
	int w = a + b + c;
	int h = font->getTall();
	drawSetTextFont( Scheme::SF_PRIMARY2 );

	int red, green, blue, alpha;

	for( int j = 0; j < gridSize[1]; j++ )
	{
		for( int i = 0; i < gridSize[0]; i++ )
		{
			char ch = grid[(j * gridSize[0] + i) * 7];

			if( ch )
			{
				getFgColor( red, green, blue, alpha );
				drawSetTextColor( red, green, blue, alpha );
				drawPrintChar( w * i, h * j, ch );
			}
		}
	}
}

TextPanel::TextPanel( const char *str, int x, int y, int w, int h ) : Panel( x, y, w, h ),
	textImage( new TextImage( str ))
{
	textImage->setSize( w, h );
}

void TextPanel::setText( const char *str )
{
	textImage->setText( str );
}

void TextPanel::setFont( Scheme::SchemeFont sf )
{
	textImage->setFont( sf );
}

void TextPanel::setFont( Font *f )
{
	textImage->setFont( f );
}

void TextPanel::setSize( int w, int h )
{
	Panel::setSize( w, h );
	getPaintSize( w, h );
	textImage->setSize( w, h );
}

void TextPanel::setFgColor( int r, int g, int b, int a )
{
	Panel::setFgColor( r, g, b, a );
	textImage->setColor( Color( r, g, b, a ));
}

void TextPanel::setFgColor( Scheme::SchemeColor sc )
{
	Panel::setFgColor( sc );
	textImage->setColor( Color( sc ));
}

TextImage *TextPanel::getTextImage()
{
	return textImage;
}

void TextPanel::paint()
{
	textImage->doPaint( this );
}
}
