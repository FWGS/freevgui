// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "button.h"

namespace vgui
{
class FooDefaultButtonController : public ButtonController, InputSignal
{
	Button *_button;
public:
	FooDefaultButtonController( Button *b ) : _button( b ) {}

	void addSignals( Button *b ) override
	{
		b->addInputSignal( this );
	}

	void removeSignals( Button *b ) override
	{
		b->removeInputSignal( this );
	}

	virtual void cursorMoved( int, int, Panel * ) override {}
	virtual void cursorEntered( Panel * ) override {}
	virtual void cursorExited( Panel * ) override {}
	virtual void mousePressed( MouseCode code, Panel * ) override
	{
		if( _button->isEnabled() && _button->isMouseClickEnabled( code ))
		{
			_button->setSelected( true );
			_button->repaint();
		}
	}

	virtual void mouseDoublePressed( MouseCode, Panel * ) override {}

	virtual void mouseReleased( MouseCode code, Panel * ) override
	{
		if( _button->isEnabled() && _button->isMouseClickEnabled( code ))
		{
			_button->setSelected( false );
			_button->fireActionSignal();
			_button->repaint();
		}
	}

	virtual void mouseWheeled( int, Panel * ) override {}
	virtual void keyPressed( KeyCode, Panel * ) override {}
	virtual void keyTyped( KeyCode, Panel * ) override {}
	virtual void keyReleased( KeyCode, Panel * ) override {}
	virtual void keyFocusTicked( Panel * ) override {}
};

void ButtonGroup::addButton( Button *b )
{
	_buttonDar.addElement( b );
}

void ButtonGroup::setSelected( Button *b )
{
	for( int i = 0; i < _buttonDar.getCount(); i++ )
	{
		if( _buttonDar[i] != b )
			_buttonDar[i]->setSelectedDirect( false );
	}

	b->setSelectedDirect( true );
}

void Button::init()
{
	_buttonController = nullptr;
	_buttonGroup = nullptr;
	_armed = false;
	_selected = false;
	_buttonBorderEnabled = true;
	_mouseClickMask = 0;
	setMouseClickEnabled( MOUSE_LEFT, true );
	setButtonController( new FooDefaultButtonController( this ));
}

void Button::setButtonController( ButtonController *bc )
{
	if( _buttonController )
		_buttonController->removeSignals( this );

	_buttonController = bc;
	bc->addSignals( this );
}

void Button::paintBackground()
{
	int w, h;

	getPaintSize( w, h );

	if( !isSelected( ))
	{
		drawSetColor( Scheme::SC_SECONDARY3 );
		drawFilledRect( 0, 0, w, h );

		if( _buttonBorderEnabled )
		{
			drawSetColor( Scheme::SC_SECONDARY1 );
			drawFilledRect( 0, 0, _size[0] - 1, 1 );
			drawFilledRect( 2, _size[1] - 2, _size[0] - 1, _size[1] - 1 );
			drawFilledRect( 0, 1, 1, _size[1] - 1 );
			drawFilledRect( _size[0] - 2, 2, _size[0] - 1, _size[1] - 2 );
			drawSetColor( Scheme::SC_WHITE );
			drawFilledRect( 1, 1, _size[0] - 2, 2 );
			drawFilledRect( 1, _size[1] - 1, _size[0], _size[1] );
			drawFilledRect( 1, 2, 2, _size[1] - 2 );
			drawFilledRect( _size[0] - 1, 1, _size[0], _size[1] - 1);
		}
	}
	else
	{
		drawSetColor( Scheme::SC_SECONDARY2 );
		drawFilledRect( 0, 0, w, h );
	}

	if( isArmed( ))
	{
		drawSetColor( Scheme::SC_WHITE );
		drawFilledRect( 0, 0, w, 2);
		drawFilledRect( 0, 2, 2, h );
		drawSetColor( Scheme::SC_SECONDARY2 );
		drawFilledRect( 2, h - 2, w, h );
		drawFilledRect( w - 2, 2, w, h - 1 );
		drawSetColor( Scheme::SC_SECONDARY1 );
		drawFilledRect( 1, h - 1, w, h );
		drawFilledRect( w - 1, 1, w, h - 1);
	}
}

Button::Button( const char *text, int x, int y, int w, int h ) :
	Label( text, x, y, w, h )
{
	init();
}

Button::Button( const char *text, int x, int y ) :
	Label( text, x, y )
{
	init();
}

void Button::setSelected( bool select )
{
	if( _buttonGroup )
		_buttonGroup->setSelected( this );
	setSelectedDirect( select );
}

void Button::setSelectedDirect( bool select )
{
	_selected = select;
	repaint();
}

void Button::setArmed( bool armed )
{
	_armed = armed;
	repaint();
}

bool Button::isSelected()
{
	return _selected;
}

void Button::doClick()
{
	setSelected( true );
	fireActionSignal();
	setSelected( false );
}

void Button::addActionSignal( ActionSignal *s )
{
	_actionSignalDar.addElement( s );
}

void Button::setButtonGroup( ButtonGroup *bg )
{
	_buttonGroup = bg;

	if( bg )
		bg->addButton( this );
}

bool Button::isArmed()
{
	return _armed;
}

void Button::setButtonBorderEnabled( bool enable )
{
	_buttonBorderEnabled = enable;
	repaint();
}

void Button::setMouseClickEnabled( MouseCode code, bool enable )
{
	if( enable )
		SetBits( _mouseClickMask, (int)( code + 1 ));
	else
		ClearBits( _mouseClickMask, (int)( code + 1 ));
}

bool Button::isMouseClickEnabled( MouseCode code )
{
	return FBitSet( _mouseClickMask, (int)( code + 1 ));
}

void Button::fireActionSignal()
{
	for( int i = 0; i < _actionSignalDar.getCount(); i++ )
		_actionSignalDar[i]->actionPerformed( this );
}

Panel *Button::createPropertyPanel()
{
	vgui_printf( "%s: UNDONE\n", PRETTY_FUNCTION );
	return NULL;
}

ToggleButton::ToggleButton( const char *text, int x, int y ) :
	Button( text, x, y )
{
}

ToggleButton::ToggleButton( const char *text, int x, int y, int w, int h ) :
	Button( text, x, y, w, h )
{
}

CheckButton::CheckButton( const char *text, int x, int y ) :
	ToggleButton( text, x, y )
{
}

CheckButton::CheckButton( const char *text, int x, int y, int w, int h ) :
	ToggleButton( text, x, y, w, h )
{
}

void CheckButton::paintBackground()
{
}

RadioButton::RadioButton( const char *text, int x, int y ) :
	ToggleButton( text, x, y )
{
}

RadioButton::RadioButton( const char *text, int x, int y, int w, int h ) :
	ToggleButton( text, x, y, w, h )
{
}

void RadioButton::paintBackground()
{
}

}
