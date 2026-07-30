// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_DESKTOP_H
#define VGUI_DESKTOP_H

#include "panel.h"
#include "controls/frame.h"

namespace vgui
{
class CLASSEXPORT MiniApp
{
public:
	MiniApp();

	virtual void getName( char*, int );
	virtual Frame* createInstance() = 0;
protected:
	virtual void setName( const char* );
	char* _name;
};
CHECK_STRUCT_SIZE( MiniApp, 8 );

class CLASSEXPORT TaskBar : public Panel
{
public:
	TaskBar( int, int, int, int );
	virtual void addFrame( Frame* );
protected:
	virtual void performLayout() override;

	class Dar<Frame*> _frameDar;
	class Dar<Button*> _taskButtonDar;
	class Panel* _tray;
};
CHECK_STRUCT_SIZE( TaskBar, 216 );

class Desktop;
class CLASSEXPORT DesktopIcon : public Panel
{
public:
	DesktopIcon( MiniApp*, Image* );
	virtual void doActivate();
	virtual void setImage( Image* );
	virtual void setDesktop( Desktop* );
	virtual MiniApp* getMiniApp();
protected:
	virtual void paintBackground() override;

	Desktop* _desktop;
	MiniApp* _miniApp;
	Image*   _image;
};
CHECK_STRUCT_SIZE( DesktopIcon, 200 );

class CLASSEXPORT Desktop : public Panel
{
public:
	Desktop( int, int, int, int );
	virtual void setSize( int, int ) override;
	virtual void iconActivated( DesktopIcon* );
	virtual void addIcon( DesktopIcon* );
	virtual void arrangeIcons();
	virtual Panel* getBackground();
	virtual Panel* getForeground();

protected:

	Panel* _background;
	Panel* _foreground;
	TaskBar* _taskBar;
	Dar<DesktopIcon*> _desktopIconDar;
	int _cascade[2];
};
CHECK_STRUCT_SIZE( Desktop, 220 );

}

#endif // VGUI_DESKTOP_H
