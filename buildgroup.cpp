// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "buildgroup.h"
#include "vgui_internal.h"
#include "panel.h"
#include "app.h"
#include "controls/label.h"
#include "image.h"

using namespace vgui;

BuildGroup::BuildGroup() :
	_enabled( false ), _snapX( 4 ), _snapY( 4 ),
	_cursor_sizenwse( new Cursor( Cursor::DC_SIZENWSE )),
	_cursor_sizenesw( new Cursor( Cursor::DC_SIZENESW )),
	_cursor_sizewe( new Cursor( Cursor::DC_SIZEWE )),
	_cursor_sizens( new Cursor( Cursor::DC_SIZENS )),
	_cursor_sizeall( new Cursor( Cursor::DC_SIZEALL )),
	_dragging( false ),
	_currentPanel( nullptr )
{

}

void BuildGroup::setEnabled(bool enable)
{
	if( _enabled != enable )
	{
		if( _currentPanel )
		{
			_currentPanel = nullptr;
			fireCurrentPanelChangeSignal();
		}
		_currentPanel = nullptr;
	}

	_enabled = enable;
}

bool BuildGroup::isEnabled()
{
	return _enabled;
}

void BuildGroup::addCurrentPanelChangeSignal(ChangeSignal *signal)
{
	_currentPanelChangeSignalDar.putElement( signal );
}

Panel *BuildGroup::getCurrentPanel()
{
	return _currentPanel;
}

void BuildGroup::copyPropertiesToClipboard()
{
	char text[32768];

	text[0] = '\0';
	for( int i = 0; i < _panelDar.getCount(); i++ )
	{
		char buf[512];
		_panelDar[i]->getPersistanceText( buf, sizeof( buf ));

		Q_strncat( text, _panelNameDar[i], sizeof( text ));
		Q_strncat( text, buf, sizeof( text ));
	}

	App::getInstance()->setClipboardText( text, strlen( text ));

	vgui_printf( "Copied to clipboard\n" );
}

void BuildGroup::applySnap( Panel *p )
{
	int x, y, w, h;

	p->getBounds( x, y, w, h );
	x = _snapX * ( x / _snapX );
	y = _snapY * ( y / _snapY );
	p->setPos( x, y );

	w = _snapX * (( x + w ) / _snapX ) - x;
	h = _snapY * (( y + h ) / _snapY ) - y;

	p->setSize( w, h );
}

void BuildGroup::fireCurrentPanelChangeSignal()
{
	for( int i = 0; i < _currentPanelChangeSignalDar.getCount(); i++ )
		_currentPanelChangeSignalDar[i]->valueChanged( nullptr );
}

void BuildGroup::panelAdded(Panel *p, const char *str)
{
	_panelDar.addElement( p );
	_panelNameDar.addElement( vgui_strdup( str ));
}

void BuildGroup::cursorMoved( int x, int y, Panel *p )
{
	if( !_dragging ) return;

	p->getApp()->getCursorPos( x, y );
	if( _dragMouseCode == MOUSE_RIGHT )
	{
		p->setSize( x - _dragStartPanelPos[0], y - _dragStartPanelPos[1] );
	}
	else
	{
		p->setPos( _dragStartPanelPos[0] + ( x - _dragStartPanelPos[0] ),
				_dragStartPanelPos[1] + ( y - _dragStartPanelPos[1] ));
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

	_dragMouseCode = code;
	_dragging = true;

	p->requestFocus();

	p->getApp()->getCursorPos( x, y );
	Vector2Set( _dragStartCursorPos, x, y );

	p->getPos( x, y );
	Vector2Set( _dragStartPanelPos, x, y );

	p->getApp()->setMouseCapture( p );
	if( _currentPanel != p )
	{
		_currentPanel = p;
		fireCurrentPanelChangeSignal();
	}

	return;
}

void BuildGroup::mouseReleased( MouseCode code, Panel *p )
{
	_dragging = false;
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

	int snapX = 0, snapY = 0;
	switch( code )
	{
	case KEY_C:
		if( ctrlDown )
			copyPropertiesToClipboard();
		break;
	case KEY_LEFT:
		snapX = -_snapX;
		break;
	case KEY_RIGHT:
		snapX = +_snapX;
		break;
	case KEY_UP:
		snapY = -_snapY;
		break;
	case KEY_DOWN:
		snapY = +_snapY;
		break;
	}

	if( snapX != 0 || snapY != 0 )
	{
		int x, y, w, h;

		p->getBounds( x, y, w, h );

		if( shiftDown )
			p->setSize( w + snapX, h + snapY );
		else
			p->setPos( x + snapX, h + snapY );

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
			return _cursor_sizenwse;
		else if( y < h - 4 )
			return _cursor_sizewe;

		return _cursor_sizenesw;
	}

	return _cursor_sizeall;
}
