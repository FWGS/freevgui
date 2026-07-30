// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/frame.h"

using namespace vgui;

Frame::Frame( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void Frame::setSize( int wide, int tall )
{
}

void Frame::setInternal( bool state )
{
}

void Frame::paintBackground()
{
}

bool Frame::isInternal()
{
	return false;
}

Panel *Frame::getClient()
{
	return NULL;
}

void Frame::setTitle( const char *title )
{
}

void Frame::getTitle( char *buf, int bufLen )
{
}

void Frame::setMoveable( bool state )
{
}

void Frame::setSizeable( bool state )
{
}

bool Frame::isMoveable()
{
	return false;
}

bool Frame::isSizeable()
{
	return false;
}

void Frame::addFrameSignal( FrameSignal *s )
{
}

void Frame::setVisible( bool state )
{
}

void Frame::setMenuButtonVisible( bool state )
{
}

void Frame::setTrayButtonVisible( bool state )
{
}

void Frame::setMinimizeButtonVisible( bool state )
{
}

void Frame::setMaximizeButtonVisible( bool state )
{
}

void Frame::setCloseButtonVisible( bool state )
{
}

void Frame::fireClosingSignal()
{
}

void Frame::fireMinimizingSignal()
{
}
