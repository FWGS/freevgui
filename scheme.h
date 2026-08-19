// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_SCHEME_H
#define VGUI_SCHEME_H

#include "vgui.h"
#include "input.h"

namespace vgui
{
class Font;

class CLASSEXPORT Scheme
{
public:
	enum SchemeColor : int32_t
	{
		SC_USER = 0,
		SC_BLACK,
		SC_WHITE,
		SC_PRIMARY1,
		SC_PRIMARY2,
		SC_PRIMARY3,
		SC_SECONDARY1,
		SC_SECONDARY2,
		SC_SECONDARY3,
		SC_COUNT,
	};

	enum SchemeFont : int32_t
	{
		SF_USER = 0,
		SF_PRIMARY1,
		SF_PRIMARY2,
		SF_PRIMARY3,
		SF_SECONDARY,
		SF_COUNT,
	};

	enum SchemeCursor : int32_t
	{
		// same as default cursor
		SCU_COUNT = Cursor::DefaultCursor::DC_LAST,
	};

	Scheme();
	virtual void setColor( SchemeColor sc, int r, int g, int b, int a );
	virtual void getColor( SchemeColor sc, int &r, int &g, int &b, int &a );
	virtual void setFont( SchemeFont sf, Font *font );
	virtual Font *getFont( SchemeFont sf );
	virtual void setCursor( SchemeCursor scu, Cursor *cursor );
	virtual Cursor *getCursor( SchemeCursor scu );

private:
	int _color[SC_COUNT][4];
	Font *_font[SF_COUNT];
	Cursor *_cursor[SCU_COUNT];
};

CHECK_STRUCT_SIZE( Scheme, 224, 304, 304 );

}

#endif // VGUI_SCHEME_H
