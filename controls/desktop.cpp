// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/desktop.h"

using namespace vgui;

MiniApp::MiniApp()
{
}

void MiniApp::getName( char *buf, int bufLen )
{
}

void MiniApp::setName( const char *name )
{
}

TaskBar::TaskBar( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void TaskBar::addFrame( Frame *frame )
{
}

void TaskBar::performLayout()
{
}

DesktopIcon::DesktopIcon( MiniApp *miniApp, Image *image ) : Panel()
{
}

void DesktopIcon::doActivate()
{
}

void DesktopIcon::setImage( Image *image )
{
}

void DesktopIcon::setDesktop( Desktop *desktop )
{
}

MiniApp *DesktopIcon::getMiniApp()
{
	return NULL;
}

void DesktopIcon::paintBackground()
{
}

Desktop::Desktop( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void Desktop::setSize( int wide, int tall )
{
}

void Desktop::iconActivated( DesktopIcon *icon )
{
}

void Desktop::addIcon( DesktopIcon *icon )
{
}

void Desktop::arrangeIcons()
{
}

Panel *Desktop::getBackground()
{
	return NULL;
}

Panel *Desktop::getForeground()
{
	return NULL;
}
