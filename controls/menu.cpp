// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/menu.h"

using namespace vgui;

Menu::Menu( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

Menu::Menu( int x, int y ) : Panel()
{
}

void Menu::addMenuItem( Panel *panel )
{
}

MenuItem::MenuItem( const char *text ) : Button( text, 0, 0 )
{
	_subMenu = NULL;
}

MenuItem::MenuItem( const char *text, Menu *menu ) : Button( text, 0, 0 )
{
	_subMenu = menu;
}

MenuSeparator::MenuSeparator( const char *text ) : Label( text )
{
}

void MenuSeparator::paintBackground()
{
}

PopupMenu::PopupMenu( int x, int y, int wide, int tall ) : Menu( x, y, wide, tall )
{
}

PopupMenu::PopupMenu( int x, int y ) : Menu( x, y )
{
}

void PopupMenu::showModal( Panel *panel )
{
}
