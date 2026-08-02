// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/list.h"
#include "controls/label.h"
#include "layout.h"
#include "signals.h"

using namespace vgui;

ListPanel::ListPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall ),
	_vpanel( new Panel( 0, 0, wide - 15, tall * 2 )),
	_scroll( new ScrollBar( wide - 15, 0, 15, tall, true ))
{
	_vpanel->setParent( this );
	_vpanel->setLayout( new StackLayout( 1, false ));
	_vpanel->setBgColor( 0, 0, 100, 0 );

	_scroll->setParent( this );
	_scroll->addIntChangeSignal( makeIntChangeHandler([this]( int value, Panel * )
	{
		setPixelScroll( -value );
	}));
}

void ListPanel::setSize( int wide, int tall )
{
	Panel::setSize( wide, tall );
	invalidateLayout( false );
}

void ListPanel::addString( const char *str )
{
	addItem( new Label( str, 0, 0, 80, 20 ));
}

void ListPanel::addItem( Panel *panel )
{
	panel->setParent( _vpanel );
	_vpanel->invalidateLayout( true );

	Panel *last = _vpanel->getChild( _vpanel->getChildCount() - 1 );
	int x, y, w, h;
	last->getBounds( x, y, w, h );

	int vw, vh;
	_vpanel->getSize( vw, vh );
	_vpanel->setSize( vw, y + h );

	_scroll->setRange( 0, y + h - _size[1] );
}

void ListPanel::setPixelScroll( int value )
{
	_vpanel->setPos( 0, value );
	repaint();
	_vpanel->repaint();
}

void ListPanel::translatePixelScroll( int delta )
{
	int x, y;

	_vpanel->getPos( x, y );
	_vpanel->setPos( 0, y + delta );
	repaint();
	_vpanel->repaint();
}

void ListPanel::performLayout()
{
	if( _vpanel->getChildCount() == 0 )
		return;

	Panel *last = _vpanel->getChild( _vpanel->getChildCount() - 1 );
	int x, y, w, h;
	last->getBounds( x, y, w, h );

	_vpanel->setSize( _size[0] - 15, y + h );
	_scroll->setBounds( _size[0] - 15, 0, 15, _size[1] );
	_scroll->setRange( 0, y + h - _size[1] );
}

void ListPanel::paintBackground()
{
	Panel::paintBackground();
}
