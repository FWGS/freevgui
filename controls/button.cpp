// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "button.h"
#include "vgui_internal.h"
#include "treefolder.h"

namespace vgui
{
class MomentaryButtonController : public ButtonController, InputSignalAdapter
{
	Button *buttons;
public:
	MomentaryButtonController( Button *b ) : buttons( b ) {}

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
		if( buttons->isEnabled() && buttons->isMouseClickEnabled( code ))
		{
			buttons->setSelected( true );
			buttons->repaint();
		}
	}

	virtual void mouseReleased( MouseCode code, Panel * ) override
	{
		if( buttons->isEnabled() && buttons->isMouseClickEnabled( code ))
		{
			buttons->setSelected( false );
			buttons->fireActionSignal();
			buttons->repaint();
		}
	}
};

// toggle controller: acts on press, not release
class LatchingButtonController : public ButtonController, InputSignalAdapter
{
	Button *buttons;
public:
	LatchingButtonController( Button *b ) : buttons( b ) {}

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
		buttons->setSelected( !buttons->isSelected( ));
		buttons->fireActionSignal();
		buttons->repaint();
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
	buttons.putElement( b );
}

void ButtonGroup::setSelected( Button *b )
{
	for( auto button : buttons )
	{
		if( button != b )
			button->setSelectedDirect( false );
	}

	b->setSelectedDirect( true );
}

void Button::init()
{
	buttonController = nullptr;
	buttonGroup = nullptr;
	armed = false;
	selected = false;
	buttonBorderEnabled = true;
	mouseClickMask = 0;
	setMouseClickEnabled( MOUSE_LEFT, true );
	setButtonController( new MomentaryButtonController( this ));
}

void Button::setButtonController( ButtonController *bc )
{
	if( buttonController )
		buttonController->removeSignals( this );

	buttonController = bc;
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

		if( buttonBorderEnabled )
		{
			drawSetColor( Scheme::SC_SECONDARY1 );
			drawFilledRect( 0, 0, size[0] - 1, 1 );
			drawFilledRect( 2, size[1] - 2, size[0] - 1, size[1] - 1 );
			drawFilledRect( 0, 1, 1, size[1] - 1 );
			drawFilledRect( size[0] - 2, 2, size[0] - 1, size[1] - 2 );
			drawSetColor( Scheme::SC_WHITE );
			drawFilledRect( 1, 1, size[0] - 2, 2 );
			drawFilledRect( 1, size[1] - 1, size[0], size[1] );
			drawFilledRect( 1, 2, 2, size[1] - 2 );
			drawFilledRect( size[0] - 1, 1, size[0], size[1] - 1);
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
	if( buttonGroup )
		buttonGroup->setSelected( this );
	setSelectedDirect( select );
}

void Button::setSelectedDirect( bool select )
{
	selected = select;
	repaint();
}

void Button::setArmed( bool state )
{
	armed = state;
	repaint();
}

bool Button::isSelected()
{
	return selected;
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

	actionSignals.putElement( s );
}

void Button::setButtonGroup( ButtonGroup *bg )
{
	buttonGroup = bg;

	if( bg )
		bg->addButton( this );
}

bool Button::isArmed()
{
	return armed;
}

void Button::setButtonBorderEnabled( bool enable )
{
	buttonBorderEnabled = enable;
	repaint();
}

void Button::setMouseClickEnabled( MouseCode code, bool enable )
{
	if( enable )
		mouseClickMask |= (int)( code + 1 );
	else
		mouseClickMask &= ~(int)( code + 1 );
}

bool Button::isMouseClickEnabled( MouseCode code )
{
	return ( mouseClickMask & (int)( code + 1 )) != 0;
}

void Button::fireActionSignal()
{
	for( auto signal : actionSignals )
		signal->actionPerformed( this );
}

Panel *Button::createPropertyPanel()
{
	Panel *p = Label::createPropertyPanel();

	TreeFolder *tf = new TreeFolder( "Button" );
	p->addChild( tf );
	tf->addChild( new Label( "setSelected" ));
	tf->addChild( new Label( "setArmed" ));

	return p;
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
