// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/menu.h"
#include "layout.h"
#include "surface.h"

using namespace vgui;

Menu::Menu( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
	setBorder( new RaisedBorder());
	setLayout( new StackLayout( 1, true ));
}

Menu::Menu( int wide, int tall ) : Panel( 0, 0, wide, tall )
{
	setBorder( new RaisedBorder());
	setLayout( new StackLayout( 1, true ));
}

void Menu::addMenuItem( Panel *panel )
{
	addChild( panel );
}

MenuItem::MenuItem( const char *text ) : Button( text, 0, 0 ), _subMenu( nullptr )
{
	setButtonBorderEnabled( false );
}

MenuItem::MenuItem( const char *text, Menu *menu ) : Button( text, 0, 0 ), _subMenu( menu )
{
	setButtonBorderEnabled( false );
}

MenuSeparator::MenuSeparator( const char *text ) : Label( text )
{
	setFont( Scheme::SF_PRIMARY3 );
}

void MenuSeparator::paintBackground()
{
	int wide, tall;
	getPaintSize( wide, tall );

	drawSetColor( Scheme::SC_SECONDARY3 );
	drawFilledRect( 0, 0, wide, tall );

	int tw, th;
	getTextSize( tw, th );

	int half = tw ? tw / 2 + 2 : 0;
	int x0 = wide / 2 - half;
	int x1 = wide / 2 + half;
	int y = tall / 2;

	drawSetColor( Scheme::SC_SECONDARY1 );
	drawFilledRect( 0, y - 1, x0, y );
	drawFilledRect( x1, y - 1, wide, y );

	drawSetColor( Scheme::SC_WHITE );
	drawFilledRect( 0, y, x0, y + 1 );
	drawFilledRect( x1, y, wide, y + 1 );
}

PopupMenu::PopupMenu( int x, int y, int wide, int tall ) : Menu( x, y, wide, tall )
{
}

PopupMenu::PopupMenu( int wide, int tall ) : Menu( wide, tall )
{
}

void PopupMenu::showModal( Panel *panel )
{
	panel->getSurfaceBase()->createPopup( this );
}
