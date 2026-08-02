// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include <stdarg.h>
#include "vgui_internal.h"
#include <stdio.h>

#include "controls/edit.h"

using namespace vgui;

EditPanel::EditPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void EditPanel::doCursorUp()
{
}

void EditPanel::doCursorDown()
{
}

void EditPanel::doCursorLeft()
{
}

void EditPanel::doCursorRight()
{
}

void EditPanel::doCursorToStartOfLine()
{
}

void EditPanel::doCursorToEndOfLine()
{
}

void EditPanel::doCursorInsertChar( char ch )
{
}

void EditPanel::doCursorBackspace()
{
}

void EditPanel::doCursorNewLine()
{
}

void EditPanel::doCursorDelete()
{
}

void EditPanel::doCursorPrintf( char *fmt, ... )
{
}

int EditPanel::getLineCount()
{
	return 0;
}

int EditPanel::getVisibleLineCount()
{
	return 0;
}

void EditPanel::setCursorBlink( bool state )
{
}

void EditPanel::setFont( Font *font )
{
}

void EditPanel::getText( int a, int b, char *buf, int bufLen )
{
}

void EditPanel::getCursorBlink( bool &blink, int &nextBlinkTime )
{
}

void EditPanel::paintBackground()
{
}

void EditPanel::paint()
{
}

void EditPanel::addLine()
{
}

Dar<char> *EditPanel::getLine( int line )
{
	return NULL;
}

void EditPanel::setChar( Dar<char> *dar, int index, char a, char b )
{
}

void EditPanel::setChar( Dar<char> *dar, int index, char a )
{
}

void EditPanel::shiftLineLeft( Dar<char> *dar, int a, int b )
{
}

void EditPanel::shiftLineRight( Dar<char> *dar, int a, int b )
{
}

int EditPanel::spatialCharOffsetBetweenTwoLines( Dar<char> *a, Dar<char> *b, int offset )
{
	return 0;
}
