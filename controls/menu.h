// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

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

class CLASSEXPORT MenuItem : public Button
{
public:
	MenuItem( const char* );
	MenuItem( const char*, Menu* );
protected:
	Menu* _subMenu;
};

class CLASSEXPORT MenuSeparator : public Label
{
public:
	MenuSeparator( const char* );
protected:
	virtual void paintBackground() override;
};

class CLASSEXPORT PopupMenu : public Menu
{
public:
	virtual void showModal( Panel* );
};
}

#endif // VGUI_MENU_H
