// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "button.h"
#include "vgui_internal.h"

namespace vgui
{
class MomentaryButtonController : public ButtonController, InputSignalAdapter
{
	Button *_button;
public:
	MomentaryButtonController( Button *b ) : _button( b ) {}

	void addSignals( Button *b ) override
	{
		b->addInputSignal( this );
	}

	void removeSignals( Button *b ) override
	{
		b->removeInputSignal( this );
	}

	virtual void mousePressed( MouseCode code, Panel * ) override
	{
		if( _button->isEnabled() && _button->isMouseClickEnabled( code ))
		{
			_button->setSelected( true );
			_button->repaint();
		}
	}

	virtual void mouseReleased( MouseCode code, Panel * ) override
	{
		if( _button->isEnabled() && _button->isMouseClickEnabled( code ))
		{
			_button->setSelected( false );
			_button->fireActionSignal();
			_button->repaint();
		}
	}
};

// toggle controller: acts on press, not release
class LatchingButtonController : public ButtonController, InputSignalAdapter
{
	Button *_button;
public:
	LatchingButtonController( Button *b ) : _button( b ) {}

	void addSignals( Button *b ) override
	{
		b->addInputSignal( this );
	}

	void removeSignals( Button *b ) override
	{
		b->removeInputSignal( this );
	}

	virtual void mousePressed( MouseCode, Panel * ) override
	{
		_button->setSelected( !_button->isSelected( ));
		_button->fireActionSignal();
		_button->repaint();
	}
};

// checkbox icon: Marlett glyphs for the box, plus the check glyph 'a' when selected
class CheckBoxImage : public Image
{
	CheckButton *cb;
public:
	CheckBoxImage( CheckButton *cb ) : cb( cb )
	{
		setSize( 20, 20 );
	}

	virtual void paint( Panel * ) override
	{
		drawSetTextFont( Scheme::SF_SECONDARY );

		// sunken Win95 box
		// back-to-front at (0,0): white plate, gray shadow, white highlight, black outline, gray face
		drawSetTextColor( Scheme::SC_WHITE );
		drawPrintChar( 0, 0, 'g' );
		drawSetTextColor( Scheme::SC_SECONDARY2 );
		drawPrintChar( 0, 0, 'c' );
		drawSetTextColor( Scheme::SC_WHITE );
		drawPrintChar( 0, 0, 'd' );
		drawSetTextColor( Scheme::SC_BLACK );
		drawPrintChar( 0, 0, 'e' );
		drawSetTextColor( Scheme::SC_SECONDARY3 );
		drawPrintChar( 0, 0, 'f' );

		if( cb->isSelected( ))
		{
			drawSetTextColor( Scheme::SC_BLACK );
			drawPrintChar( 0, 0, 'a' );
		}
	}
};

// radio icon: Marlett glyphs for the ring, plus the filled glyph 'h' when selected
class RadioButtonImage : public Image
{
	RadioButton *rb;
public:
	RadioButtonImage( RadioButton *rb ) : rb( rb )
	{
		setSize( 20, 20 );
	}

	virtual void paint( Panel * ) override
	{
		drawSetTextFont( Scheme::SF_SECONDARY );

		// sunken Win95 ring, back-to-front at (0,0): white plate, gray shadow arc, white
		// highlight arc, black outline, gray face
		drawSetTextColor( Scheme::SC_WHITE );
		drawPrintChar( 0, 0, 'n' );
		drawSetTextColor( Scheme::SC_SECONDARY2 );
		drawPrintChar( 0, 0, 'j' );
		drawSetTextColor( Scheme::SC_WHITE );
		drawPrintChar( 0, 0, 'k' );
		drawSetTextColor( Scheme::SC_BLACK );
		drawPrintChar( 0, 0, 'l' );
		drawSetTextColor( Scheme::SC_SECONDARY3 );
		drawPrintChar( 0, 0, 'm' );

		if( rb->isSelected( ))
		{
			drawSetTextColor( Scheme::SC_BLACK );
			drawPrintChar( 0, 0, 'h' );
		}
	}
};

void ButtonGroup::addButton( Button *b )
{
	_buttonDar.putElement( b );
}

void ButtonGroup::setSelected( Button *b )
{
	for( auto button : _buttonDar )
	{
		if( button != b )
			button->setSelectedDirect( false );
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
	setButtonController( new MomentaryButtonController( this ));
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

Button::Button( const char *text, int x, int y, int w, int h ) : Label( text, x, y, w, h )
{
	init();
}

Button::Button( const char *text, int x, int y ) : Label( text, x, y )
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
	if( s == nullptr )
		return;

	_actionSignalDar.putElement( s );
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
	for( auto signal : _actionSignalDar )
		signal->actionPerformed( this );
}

Panel *Button::createPropertyPanel()
{
	vgui_printf( "%s: UNDONE\n", PRETTY_FUNCTION );
	return NULL;
}

ToggleButton::ToggleButton( const char *text, int x, int y ) :
	Button( text, x, y )
{
	setButtonController( new LatchingButtonController( this ));
}

ToggleButton::ToggleButton( const char *text, int x, int y, int w, int h ) :
	Button( text, x, y, w, h )
{
	setButtonController( new LatchingButtonController( this ));
}

CheckButton::CheckButton( const char *text, int x, int y ) :
	ToggleButton( text, x, y )
{
	setTextAlignment( Label::RIGHT ); // a_east: text to the right of the box
	setImage( new CheckBoxImage( this ));

	int w, h;
	getContentSize( w, h );
	setSize( w, h );
}

CheckButton::CheckButton( const char *text, int x, int y, int w, int h ) :
	ToggleButton( text, x, y, w, h )
{
	setTextAlignment( Label::RIGHT );
	setImage( new CheckBoxImage( this ));
}

void CheckButton::paintBackground()
{
	int w, h;

	getPaintSize( w, h );
	drawSetColor( Scheme::SC_SECONDARY3 );
	drawFilledRect( 0, 0, w, h );
}

RadioButton::RadioButton( const char *text, int x, int y ) :
	ToggleButton( text, x, y )
{
	setTextAlignment( Label::RIGHT );
	setImage( new RadioButtonImage( this ));

	int w, h;
	getContentSize( w, h );
	setSize( w, h );
}

RadioButton::RadioButton( const char *text, int x, int y, int w, int h ) :
	ToggleButton( text, x, y, w, h )
{
	setTextAlignment( Label::RIGHT );
	setImage( new RadioButtonImage( this ));
}

void RadioButton::paintBackground()
{
	int w, h;

	getPaintSize( w, h );
	drawSetColor( Scheme::SC_SECONDARY3 );
	drawFilledRect( 0, 0, w, h );
}

}
