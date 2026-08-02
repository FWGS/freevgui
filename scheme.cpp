// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "scheme.h"
#include "vgui_internal.h"
#include "font.h"

using namespace vgui;

Scheme::Scheme()
{
	setColor( SC_BLACK,      0,   0,   0,   0 );
	setColor( SC_WHITE,      255, 255, 255, 0 );
	setColor( SC_PRIMARY1,   102, 102, 153, 0 );
	setColor( SC_PRIMARY2,   153, 153, 204, 0 );
	setColor( SC_PRIMARY3,   204, 204, 255, 0 );
	setColor( SC_SECONDARY1, 102, 102, 102, 0 );
	setColor( SC_SECONDARY2, 153, 153, 153, 0 );
	setColor( SC_SECONDARY3, 204, 204, 204, 0 );
	setColor( SC_USER,       0,   0,   0, 0 );
	setFont( SF_PRIMARY1,
				new Font( "Arial",    0, 0, 20, 0, 0, 400, false, false, false, false ));
	setFont( SF_PRIMARY2,
				new Font( "FixedSys", 0, 0, 18, 0, 0, 400, false, false, false, false ));
	setFont( SF_PRIMARY3,
				new Font( "Arial",    0, 0, 12, 0, 0, 400, false, false, false, false ));
	setFont( SF_SECONDARY,
				new Font( "Marlett",  0, 0, 16, 0, 0, 0,   false, false, false, false ));

	for( int i = Cursor::DC_USER; i < Cursor::DC_LAST; i++ )
		setCursor( static_cast<SchemeCursor>( i ),
					  new Cursor( static_cast<Cursor::DefaultCursor>( i )));
}

void Scheme::setColor(SchemeColor sc, int r, int g, int b, int a)
{
	Vector4Set( _color[sc], r, g, b, a );
}

void Scheme::getColor(SchemeColor sc, int &r, int &g, int &b, int &a)
{
	r = _color[sc][0];
	g = _color[sc][1];
	b = _color[sc][2];
	a = _color[sc][3];
}

void Scheme::setFont(SchemeFont sf, Font *font)
{
	if( font )
		_font[sf] = font;
}

Font *Scheme::getFont(SchemeFont sf)
{
	return _font[sf];
}

void Scheme::setCursor(SchemeCursor scu, Cursor *cursor)
{
	if( cursor )
		_cursor[scu] = cursor;
}

Cursor *Scheme::getCursor(SchemeCursor scu)
{
	return _cursor[scu];
}
