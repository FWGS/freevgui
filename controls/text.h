// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_TEXT_H
#define VGUI_TEXT_H

#include "panel.h"
#include "signals.h"

namespace vgui
{
class CLASSEXPORT TextEntry : public Panel, public InputSignal
{
public:
	TextEntry( const char*, int, int, int, int);
	virtual void setText( const char*, int );
	virtual void getText( int, char*, int);
	virtual void resetCursorBlink();
	virtual void doGotoLeft();
	virtual void doGotoRight();
	virtual void doGotoFirstOfLine();
	virtual void doGotoEndOfLine();
	virtual void doInsertChar( char );
	virtual void doBackspace();
	virtual void doDelete();
	virtual void doSelectNone();
	virtual void doCopySelected();
	virtual void doPaste();
	virtual void doPasteSelected();
	virtual void doDeleteSelected();
	virtual void addActionSignal( ActionSignal* );
	virtual void setFont( Font* );
	virtual void setTextHidden( bool );
protected:
	virtual void paintBackground() override;
	virtual void setCharAt( char, int );
	virtual void fireActionSignal();
	virtual bool getSelectedRange( int&, int& );
	virtual bool getSelectedPixelRange( int&, int& );
	virtual int cursorToPixelSpace( int );
	virtual void selectCheck();
	virtual void cursorMoved( int, int, Panel* );
	virtual void cursorEntered( Panel* );
	virtual void cursorExited( Panel* );
	virtual void mousePressed( MouseCode, Panel* );
	virtual void mouseDoublePressed( MouseCode, Panel* );
	virtual void mouseReleased( MouseCode, Panel* );
	virtual void mouseWheeled( int, Panel* );
	virtual void keyPressed( KeyCode, Panel* );
	virtual void keyTyped( KeyCode, Panel* );
	virtual void keyReleased( KeyCode, Panel* );
	virtual void keyFocusTicked( Panel* );

	Dar<char> _lineDar;
	int       _cursorPos;
	bool      _cursorBlink, _hideText;
	long int  _cursorNextBlinkTime;
	int       _cursorBlinkRate;
	int       _select[2];
	Dar<ActionSignal*> _actionSignalDar;
	Font*    _font;
};
CHECK_STRUCT_SIZE( TextEntry, 244 );

class CLASSEXPORT TextGrid : public Panel
{
public:
	TextGrid( int, int, int, int, int, int );
	virtual void setGridSize( int, int );
	virtual void newLine();
	virtual void setXY( int, int );
	virtual int vprintf( const char*, va_list );
	virtual int printf( const char*, ... );
protected:
	virtual void paintBackground() override;

	int _xy[2], _bgColor[3], _fgColor[3];
	char* _grid;
	int _gridSize[2];
};
CHECK_STRUCT_SIZE( TextGrid, 232 );

class CLASSEXPORT TextPanel : public Panel
{
public:
	TextPanel( const char*, int, int, int, int );
	virtual void setText( const char* );
	virtual void setFont( Scheme::SchemeFont );
	virtual void setFont( Font* );
	virtual void setSize( int, int ) override;
	virtual void setFgColor( int, int, int, int ) override;
	virtual void setFgColor( Scheme::SchemeColor ) override;
	virtual TextImage* getTextImage();
protected:
	virtual void paint() override;
private:
	TextImage* _textImage;
};
CHECK_STRUCT_SIZE( TextPanel, 192 );
}

#endif // VGUI_TEXT_H
