// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_EDITPANEL_H
#define VGUI_EDITPANEL_H

#include "panel.h"

namespace vgui
{
class CLASSEXPORT EditPanel : public Panel
{
public:
	EditPanel( int, int, int, int );

	virtual void doCursorUp();
	virtual void doCursorDown();
	virtual void doCursorLeft();
	virtual void doCursorRight();
	virtual void doCursorToStartOfLine();
	virtual void doCursorToEndOfLine();
	virtual void doCursorInsertChar( char );
	virtual void doCursorBackspace();
	virtual void doCursorNewLine();
	virtual void doCursorDelete();
	virtual void doCursorPrintf( char*, ... );
	virtual int getLineCount();
	virtual int getVisibleLineCount();
	virtual void setCursorBlink( bool );
	virtual void setFont( Font* );
	virtual void getText( int, int, char*, int );

	void getCursorBlink( bool&, int& );
protected:
	virtual void paintBackground() override;
	virtual void paint() override;
	virtual void addLine();
	virtual Dar<char>* getLine( int );
	virtual void setChar( Dar<char>*, int, char, char );
	virtual void setChar( Dar<char>*, int, char );
	virtual void shiftLineLeft( Dar<char>*, int, int );
	virtual void shiftLineRight( Dar<char>*, int, int );

	Dar<Dar<char>*> _lineDarDar;
	int   _cursor[2];
	bool  _cursorBlink;
	int   _cursorNextBlinkTime;
	Font* _font;
private:
	virtual int spatialCharOffsetBetweenTwoLines( Dar<char>*, Dar<char>*, int );
};
CHECK_STRUCT_SIZE( EditPanel, 220, 304, 304 );
}

#endif // VGUI_EDITPANEL_H
