// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_MENU_H
#define VGUI_MENU_H

#include "panel.h"
#include "controls/button.h"

namespace vgui
{
class CLASSEXPORT Menu : public Panel
{
public:
	Menu( int, int, int, int );
	Menu( int, int );
	virtual void addMenuItem( Panel* );
};
CHECK_STRUCT_SIZE( Menu, 188, 264, 264 );

class CLASSEXPORT MenuItem : public Button
{
public:
	MenuItem( const char* );
	MenuItem( const char*, Menu* );
protected:
	Menu* subMenu;
};
CHECK_STRUCT_SIZE( MenuItem, 244, 352, 352 );

class CLASSEXPORT MenuSeparator : public Label
{
public:
	MenuSeparator( const char* );
protected:
	virtual void paintBackground() override;
};
CHECK_STRUCT_SIZE( MenuSeparator, 208, 288, 288 );

class CLASSEXPORT PopupMenu : public Menu
{
public:
	PopupMenu( int, int, int, int );
	PopupMenu( int, int );
	virtual void showModal( Panel* );
};
CHECK_STRUCT_SIZE( PopupMenu, 188, 264, 264 );
}

#endif // VGUI_MENU_H
