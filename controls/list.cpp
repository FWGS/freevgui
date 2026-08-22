// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/list.h"
#include "controls/label.h"
#include "layout.h"
#include "signals.h"

using namespace vgui;

ListPanel::ListPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall ),
	itemPanel( new Panel( 0, 0, wide - 15, tall * 2 )),
	scrollBar( new ScrollBar( wide - 15, 0, 15, tall, true ))
{
	itemPanel->setParent( this );
	itemPanel->setLayout( new StackLayout( 1, false ));
	itemPanel->setBgColor( 0, 0, 100, 0 );

	scrollBar->setParent( this );
	scrollBar->addIntChangeSignal( makeIntChangeHandler([this]( int value, Panel * )
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
	panel->setParent( itemPanel );
	itemPanel->invalidateLayout( true );

	Panel *last = itemPanel->getChild( itemPanel->getChildCount() - 1 );
	int x, y, w, h;
	last->getBounds( x, y, w, h );

	int vw, vh;
	itemPanel->getSize( vw, vh );
	itemPanel->setSize( vw, y + h );

	scrollBar->setRange( 0, y + h - size[1] );
}

void ListPanel::setPixelScroll( int value )
{
	itemPanel->setPos( 0, value );
	repaint();
	itemPanel->repaint();
}

void ListPanel::translatePixelScroll( int delta )
{
	int x, y;

	itemPanel->getPos( x, y );
	itemPanel->setPos( 0, y + delta );
	repaint();
	itemPanel->repaint();
}

void ListPanel::performLayout()
{
	if( itemPanel->getChildCount() == 0 )
		return;

	Panel *last = itemPanel->getChild( itemPanel->getChildCount() - 1 );
	int x, y, w, h;
	last->getBounds( x, y, w, h );

	itemPanel->setSize( size[0] - 15, y + h );
	scrollBar->setBounds( size[0] - 15, 0, 15, size[1] );
	scrollBar->setRange( 0, y + h - size[1] );
}

void ListPanel::paintBackground()
{
	Panel::paintBackground();
}
