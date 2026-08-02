// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include <stdarg.h>
#include "vgui_internal.h"
#include <stdio.h>
#include "app.h"
#include "text.h"
#include "font.h"

namespace vgui
{
class FooDefaultTextEntrySignal : public FocusChangeSignal
{
	TextEntry *_te;
public:
	FooDefaultTextEntrySignal( TextEntry *te ) : _te( te ) {}
	void focusChanged( bool change, Panel *p ) override
	{
		_te->resetCursorBlink();
		_te->doSelectNone();
	}
};

TextEntry::TextEntry( const char *str, int x, int y, int w, int h ) : Panel( x, y, w, h ),
	_cursorPos( 0 ), _hideText( false ), _cursorBlinkRate( 400 )
{
	Vector2Set( _select, -1, -1 );
	resetCursorBlink();
	setText( str, strlen( str ));
	addInputSignal( this );
	addFocusChangeSignal( new FooDefaultTextEntrySignal( this ));
}

void TextEntry::setText( const char *text, int len )
{
	_lineDar.removeAll();
	_lineDar.ensureCapacity( len );

	for( int i = 0; i < len; i++ )
	{
		_lineDar.addElement( text[i] );
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
		if( i >= _lineDar.getCount())
			break;
		buf[i - off] = _lineDar[i];
	}
	buf[len - 1] = 0;
}

void TextEntry::resetCursorBlink()
{
	_cursorBlink = false;
	_cursorNextBlinkTime = getApp()->getTimeMillis() + _cursorBlinkRate;
}

void TextEntry::doGotoLeft()
{
	selectCheck();
	_cursorPos = Q_max( _cursorPos - 1, 0 );
	resetCursorBlink();
	repaint();
}

void TextEntry::doGotoRight()
{
	selectCheck();
	_cursorPos = Q_min( _cursorPos + 1, _lineDar.getCount());
	resetCursorBlink();
	repaint();
}

void TextEntry::doGotoFirstOfLine()
{
	selectCheck();
	_cursorPos = 0;
	resetCursorBlink();
	repaint();
}

void TextEntry::doGotoEndOfLine()
{
	selectCheck();
	_cursorPos = _lineDar.getCount();
	resetCursorBlink();
	repaint();
}

void TextEntry::doInsertChar( char ch )
{
	_lineDar.setCount( _lineDar.getCount() + 1 );
	for( int i = _lineDar.getCount() - 1; i >= _cursorPos; i++ )
		setCharAt( _lineDar[i], i + 1 );
	setCharAt( ch, _cursorPos );
	_cursorPos++;
	resetCursorBlink();
	repaint();
}

void TextEntry::doBackspace()
{
	if( !_cursorPos || !_lineDar.getCount( ))
		return;

	for( int i = _cursorPos; i < _lineDar.getCount(); i++ )
		setCharAt( _lineDar[i],	i - 1 );

	_lineDar.setCount( _lineDar.getCount() - 1 );
	_cursorPos--;
	resetCursorBlink();
	repaint();
}

void TextEntry::doDelete()
{
	if( !_lineDar.getCount() || _cursorPos == _lineDar.getCount( ))
		return;

	for( int i = _cursorPos + 1; i < _lineDar.getCount(); i++ )
		setCharAt( _lineDar[i], i - 1 );

	_lineDar.setCount( _lineDar.getCount() - 1 );
	_cursorPos--;
	resetCursorBlink();
	repaint();
}

void TextEntry::doSelectNone()
{
	_select[0] = -1;
	repaint();
}

void TextEntry::doCopySelected()
{
	int x, y;

	if( !getSelectedRange( x, y ))
		return;

	char buf[256];
	int i = 0;

	for( ; i < sizeof( buf ) - 1; i++ )
	{
		if( x + i >= y )
			break;

		buf[i] = _lineDar[x + i];
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
	_actionSignalDar.putElement( as );
}

void TextEntry::setFont( Font *f )
{
	_font = f;
}

void TextEntry::setTextHidden( bool hide )
{
	_hideText = hide;
	repaint();
}

void TextEntry::paintBackground()
{
	Font *font = _font ? _font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );
	int text_h = font->getTall();

	{
		int x, y;
		if( getSelectedRange( x, y ))
		{
			drawSetColor( Scheme::SC_WHITE );
			drawFilledRect( 0, 0, x, text_h + 1 );
			drawFilledRect( x, 0, _size[0], text_h + 1 );
			drawSetColor( 0, 0, 200, 0 );
			drawFilledRect( x, 0, y, text_h + 1 );
		}
		else
		{
			drawSetColor(Scheme::SC_WHITE);
			drawFilledRect(0,0,_size[0],_size[1]);
		}
	}

	drawSetTextFont( font );
	drawSetColor( Scheme::SC_BLACK );
	drawSetTextPos( 3, 0 );

	for( int i = 0; i < _lineDar.getCount(); i++ )
	{
		if( _hideText )
			drawPrintChar( '*' );
		else
			drawPrintChar( _lineDar[i] );
	}

	if( hasFocus( ))
	{
		drawSetColor( Scheme::SC_BLACK );
		drawFilledRect( 0, 0, _size[0], 1 );
		drawFilledRect( 0, _size[1] - 1, _size[0], _size[1] );
		drawFilledRect( 0, 1, 1, _size[1] - 1 );
		drawFilledRect( _size[0] - 1, 1, _size[0], _size[1] - 1 );

		if( !_cursorBlink )
		{
				int x = cursorToPixelSpace( _cursorPos );
				drawSetColor( Scheme::SC_BLACK );
				drawFilledRect( x + 3, 2, x + 4, text_h - 1 );
		}
	}
}

void TextEntry::setCharAt( char ch, int at )
{
	if( at < 0 )
		return;

	_lineDar.ensureCapacity( at + 1 );
	_lineDar.setElementAt( ch, at );
}

void TextEntry::fireActionSignal()
{
	for( int i = 0; i < _actionSignalDar.getCount(); i++ )
		_actionSignalDar[i]->actionPerformed( this );
}

bool TextEntry::getSelectedRange( int &x, int &y )
{
	if( _select[0] == -1 )
		return false;

	x = _select[0];
	y = _select[1];
	if(	x < y )
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
	Font *font = _font ? _font : getApp()->getScheme()->getFont( Scheme::SF_PRIMARY1 );

	int x = 0;
	for( int i = 0; i < _lineDar.getCount(); i++ )
	{
		if( i == _cursorPos )
			break;

		int a, b, c;
		if( _hideText )
			font->getCharABCwide( '*', a, b, c );
		else font->getCharABCwide( _lineDar[i], a, b, c );

		x += a + b + c;
	}

	return x;
}

void TextEntry::selectCheck()
{
	if( isKeyDown( KEY_LSHIFT ) || isKeyDown( KEY_RSHIFT ))
		_select[0] = _cursorPos;
	else _select[0] = -1;
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
		case KEY_C: doCopySelected(); break;
		case KEY_V: doPaste(); break;
		}
	}
	else
	{
		char ch;

		switch( code )
		{
		case KEY_INSERT: if( shift ) doPaste(); break;
		case KEY_DELETE: shift ? doDeleteSelected() : doDelete(); break;
		case KEY_LEFT: doGotoLeft(); break;
		case KEY_RIGHT: doGotoRight(); break;
		case KEY_HOME: doGotoFirstOfLine(); break;
		case KEY_END: doGotoEndOfLine(); break;
		case KEY_BACKSPACE: doBackspace(); break;
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

	_select[1] = _cursorPos;

	if( code == KEY_ENTER )
		fireActionSignal();
}

void TextEntry::keyReleased( KeyCode, Panel* )
{

}

void TextEntry::keyFocusTicked( Panel* )
{
	int time = getApp()->getTimeMillis();
	if( time > _cursorNextBlinkTime )
	{
		_cursorBlink = !_cursorBlink;
		_cursorNextBlinkTime = time + _cursorBlinkRate;
		repaint();
	}
}

TextGrid::TextGrid( int grid_w, int grid_h, int x, int y, int w, int h )
	: Panel( x, y, w, h ),
	  _grid( nullptr )
{
	Vector2Set( _gridSize, 0, 0 );
	setGridSize( grid_w, grid_h );
	Vector2Set( _xy, 0, 0 );

	setBgColor( 255, 255, 255, 0 );
	setFgColor( 0, 0, 0, 0 );
}

void TextGrid::setGridSize( int w, int h )
{
	if( w <= 0 || h <= 0 )
		return;

	delete[] _grid;
	_grid = new char[w * h * 7];
	memset( _grid, 0, w * h * 7 );
	Vector2Set( _gridSize, w, h );
}

void TextGrid::newLine()
{
	if( _xy[1] == _gridSize[1] - 1 )
	{
		if( _gridSize[1] > 1 )
		{
			size_t lineSize = _gridSize[0] * 7;

			memset( &_grid[( _xy[1] * _gridSize[0] + _xy[0] ) * 7], 0, _gridSize[0] - _xy[0] );
			for( int j = 1; j < _gridSize[1]; j++ )
			{
				memcpy( &_grid[( j - 1 ) * lineSize], &_grid[j * lineSize], lineSize );
			}
			memset( &_grid[_xy[1] * lineSize], 0, lineSize );
		}

		_xy[0] = 0;
	}
	else
	{
		_xy[0] = 0;
		++_xy[1];
	}
}

void TextGrid::setXY( int x, int y )
{
	Vector2Set( _xy, x, y );
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

		if( _xy[0] >= 0 && _xy[0] < _gridSize[0] )
		{
			if( _xy[1] >= 0 && _xy[1] < _gridSize[1] )
			{
				_grid[(_xy[1] * _gridSize[0] + _xy[0]) * 7] = buf[i];
				++_xy[0];
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

	for( int j = 0; j < _gridSize[1]; j++ )
	{
		for( int i = 0; i < _gridSize[0]; i++ )
		{
			char ch = _grid[(j * _gridSize[0] + i) * 7];

			if( ch )
			{
				getFgColor( red, green, blue, alpha );
				drawSetTextColor( red, green, blue, alpha );
				drawPrintChar( w * i, h * j, ch );
			}
		}
	}
}

TextPanel::TextPanel( const char *str, int x, int y, int w, int h ) :
	Panel( x, y, w, h ),
	_textImage( new TextImage( str ))
{
	_textImage->setSize( w, h );
}

void TextPanel::setText( const char *str )
{
	_textImage->setText( str );
}

void TextPanel::setFont( Scheme::SchemeFont sf )
{
	_textImage->setFont( sf );
}

void TextPanel::setFont( Font *f )
{
	_textImage->setFont( f );
}

void TextPanel::setSize( int w, int h )
{
	Panel::setSize( w, h );
	getPaintSize( w, h );
	_textImage->setSize( w, h );
}

void TextPanel::setFgColor( int r, int g, int b, int a )
{
	Panel::setFgColor( r, g, b, a );
	_textImage->setColor( Color( r, g, b, a ));
}

void TextPanel::setFgColor( Scheme::SchemeColor sc )
{
	Panel::setFgColor( sc );
	_textImage->setColor( Color( sc ));
}

TextImage *TextPanel::getTextImage()
{
	return _textImage;
}

void TextPanel::paint()
{
	_textImage->doPaint( this );
}

}
