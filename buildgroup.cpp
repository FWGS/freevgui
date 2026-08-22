// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "buildgroup.h"
#include "vgui_internal.h"
#include "panel.h"
#include "app.h"
#include "controls/label.h"
#include "image.h"

using namespace vgui;

BuildGroup::BuildGroup() :
	enabled( false ), snapX( 4 ), snapY( 4 ),
	sizeNwSeCursor( new Cursor( Cursor::DC_SIZENWSE )),
	sizeNeSwCursor( new Cursor( Cursor::DC_SIZENESW )),
	sizeWeCursor( new Cursor( Cursor::DC_SIZEWE )),
	sizeNsCursor( new Cursor( Cursor::DC_SIZENS )),
	sizeAllCursor( new Cursor( Cursor::DC_SIZEALL )),
	dragging( false ),
	selectedPanel( nullptr )
{

}

void BuildGroup::setEnabled(bool enable)
{
	if( enabled != enable )
	{
		if( selectedPanel )
		{
			selectedPanel = nullptr;
			fireCurrentPanelChangeSignal();
		}
		selectedPanel = nullptr;
	}

	enabled = enable;
}

bool BuildGroup::isEnabled()
{
	return enabled;
}

void BuildGroup::addCurrentPanelChangeSignal(ChangeSignal *signal)
{
	selectedPanelChangeSignals.putElement( signal );
}

Panel *BuildGroup::getCurrentPanel()
{
	return selectedPanel;
}

void BuildGroup::copyPropertiesToClipboard()
{
	char text[32768];

	text[0] = '\0';
	for( int i = 0; i < panels.getCount(); i++ )
	{
		char buf[512];
		panels[i]->getPersistanceText( buf, sizeof( buf ));

		strncat( text, panelNames[i], sizeof( text ));
		strncat( text, buf, sizeof( text ));
	}

	App::getInstance()->setClipboardText( text, strlen( text ));

	vgui_printf( "Copied to clipboard\n" );
}

void BuildGroup::applySnap( Panel *p )
{
	int x, y, w, h;

	p->getBounds( x, y, w, h );
	x = snapX * ( x / snapX );
	y = snapY * ( y / snapY );
	p->setPos( x, y );

	w = snapX * (( x + w ) / snapX ) - x;
	h = snapY * (( y + h ) / snapY ) - y;

	p->setSize( w, h );
}

void BuildGroup::fireCurrentPanelChangeSignal()
{
	for( int i = 0; i < selectedPanelChangeSignals.getCount(); i++ )
		selectedPanelChangeSignals[i]->valueChanged( nullptr );
}

void BuildGroup::panelAdded(Panel *p, const char *str)
{
	panels.addElement( p );
	panelNames.addElement( vgui_strdup( str ));
}

void BuildGroup::cursorMoved( int x, int y, Panel *p )
{
	if( !dragging ) return;

	p->getApp()->getCursorPos( x, y );
	if( dragMouseCode == MOUSE_RIGHT )
	{
		p->setSize( x - dragStartPanelPos[0], y - dragStartPanelPos[1] );
	}
	else
	{
		p->setPos( dragStartPanelPos[0] + ( x - dragStartCursorPos[0] ),
				dragStartPanelPos[1] + ( y - dragStartCursorPos[1] ));
	}
	applySnap( p );

	p->repaintParent();
}

void BuildGroup::mousePressed( MouseCode code, Panel *p )
{
	int x, y;

	if( code == MOUSE_RIGHT )
	{
		p->getApp()->getCursorPos( x, y );

		p->screenToLocal( x, y );

		Label *label = new Label( "Label", x, y, 0, 0 );
		label->setBorder( new LineBorder( ));

		label->setParent( p );
		label->setBuildGroup( this, "Label" );
	}

	dragMouseCode = code;
	dragging = true;

	p->requestFocus();

	p->getApp()->getCursorPos( x, y );
	dragStartCursorPos[0] = x;
	dragStartCursorPos[1] = y;

	p->getPos( x, y );
	dragStartPanelPos[0] = x;
	dragStartPanelPos[1] = y;

	p->getApp()->setMouseCapture( p );
	if( selectedPanel != p )
	{
		selectedPanel = p;
		fireCurrentPanelChangeSignal();
	}

	return;
}

void BuildGroup::mouseReleased( MouseCode code, Panel *p )
{
	dragging = false;
	p->getApp()->setMouseCapture( nullptr );
}

void BuildGroup::mouseDoublePressed(MouseCode, Panel *)
{
	return;
}

void BuildGroup::keyTyped( KeyCode code, Panel *p )
{
	bool shiftDown = p->isKeyDown( KEY_LSHIFT ) || p->isKeyDown( KEY_RSHIFT );
	bool ctrlDown = p->isKeyDown( KEY_LCONTROL ) || p->isKeyDown( KEY_RCONTROL );

	int dx = 0, dy = 0;
	switch( code )
	{
	case KEY_C:
		if( ctrlDown )
			copyPropertiesToClipboard();
		break;
	case KEY_LEFT:
		dx = -snapX;
		break;
	case KEY_RIGHT:
		dx = +snapX;
		break;
	case KEY_UP:
		dy = -snapY;
		break;
	case KEY_DOWN:
		dy = +snapY;
		break;
	}

	if( dx != 0 || dy != 0 )
	{
		int x, y, w, h;

		p->getBounds( x, y, w, h );

		if( shiftDown )
			p->setSize( w + dx, h + dy );
		else
			p->setPos( x + dx, y + dy );

		applySnap( p );
		p->repaint();

		if( p->getParent( ))
			p->getParent()->repaint();
	}
}

Cursor *BuildGroup::getCursor( Panel *p )
{
	int x, y, w, h;

	p->getApp()->getCursorPos( x, y );
	p->screenToLocal( x, y );
	p->getSize( w, h );

	if( x < 2 )
	{
		if( y < 4 )
			return sizeNwSeCursor;
		else if( y < h - 4 )
			return sizeWeCursor;

		return sizeNeSwCursor;
	}

	return sizeAllCursor;
}
