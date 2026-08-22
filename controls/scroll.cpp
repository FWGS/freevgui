// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "app.h"
#include "vgui_internal.h"
#include "signals.h"
#include "scroll.h"

namespace vgui
{
class DefaultSliderSignal : public InputSignalAdapter
{
	Slider *slider;
public:
	DefaultSliderSignal( Slider *slider ) : slider( slider )
	{
	}

	virtual void cursorMoved( int x, int y, Panel *p ) override
	{
		slider->privateCursorMoved( x, y, p );
	}
	virtual void mousePressed( MouseCode code, Panel *p ) override
	{
		slider->privateMousePressed( code, p );
	}
	virtual void mouseReleased( MouseCode code, Panel *p ) override
	{
		slider->privateMouseReleased( code, p );
	}
};

Slider::Slider( int x, int y, int w, int h, bool vertical ) : Panel( x, y, w, h ),
	vertical( vertical ),
	dragging( false ),
	value( 0 ),
	rangeWindow( 0 ),
	rangeWindowEnabled( false ),
	buttonOffset( 0 )
{
	range[0] = 0;
	range[1] = 299;
	recomputeNobPosFromValue();
	addInputSignal( new DefaultSliderSignal( this ));
}

void Slider::setValue(int newValue)
{
	int orig_value = value;

	newValue = bound( range[0], newValue, range[1] );

	value = newValue;
	recomputeNobPosFromValue();

	if( value != orig_value )
		fireIntChangeSignal();
}

int Slider::getValue()
{
	return value;
}

bool Slider::isVertical()
{
	return vertical;
}

void Slider::addIntChangeSignal(IntChangeSignal *s)
{
	intChangeSignals.putElement( s );
}

void Slider::setRange(int imin, int imax)
{
	if( imax < imin )
		imax = imin;

	range[0] = imin;
	range[1] = imax;
}

void Slider::getRange(int &imin, int &imax)
{
	imin = range[0];
	imax = range[1];
}

void Slider::setRangeWindow(int newRangeWindow)
{
	rangeWindow = newRangeWindow;
}

void Slider::setRangeWindowEnabled(bool enable)
{
	rangeWindowEnabled = enable;
}

void Slider::setSize(int w, int h)
{
	Panel::setSize( w, h );
	recomputeNobPosFromValue();
}

void Slider::getNobPos(int &imin, int &imax)
{
	imin = knobPos[0];
	imax = knobPos[1];
}

bool Slider::hasFullRange()
{
	int w, h;
	getPaintSize( w, h );

	float span = range[1] - range[0];
	float window = rangeWindowEnabled ? rangeWindow : span;

	if( window < 0.0f )
		return false;

	float size = vertical ? h : w;
	return window <= size + buttonOffset;
}

void Slider::setButtonOffset(int off)
{
	buttonOffset = off;
}

void Slider::recomputeNobPosFromValue()
{
	int w, h;
	getPaintSize( w, h );

	float span = range[1] - range[0];
	float offset = value - range[0];
	float window = span;

	if( rangeWindow < 0.0f && rangeWindowEnabled )
	{
		repaint();
		return;
	}

	if( rangeWindow >= 0.0f && rangeWindowEnabled )
		window = rangeWindow;

	if( window > 0.0f )
	{
		float size = vertical ? h : w;
		float nobSize = size / window * size;

		knobPos[0] = ( size - nobSize ) * ( offset / span );
		knobPos[1] = knobPos[0] + nobSize;

		if( knobPos[1] > size )
		{
			knobPos[0] = size - nobSize;
			knobPos[1] = size;
		}
	}
}

void Slider::recomputeValueFromNobPos()
{
	int w, h;

	getPaintSize( w, h );

	float span = range[1] - range[0];
	float offset = value - range[0];
	float window = span;

	if( rangeWindow < 0.0f && rangeWindowEnabled )
	{
		value = bound( range[0], (int)( range[0] + offset + 0.5f ), range[1] );
		return;
	}

	if( rangeWindow >= 0.0f && rangeWindowEnabled )
		window = rangeWindow;

	if( window > 0 )
	{
		float size = vertical ? h : w;
		offset = knobPos[0] / ( size - ( size / window ) * size ) * span;
	}

	value = bound( range[0], (int)( range[0] + offset + 0.5f ), range[1] );
}

void Slider::privateCursorMoved( int x, int y, Panel *p )
{
	if( dragging == false )
		return;

	getApp()->getCursorPos( x, y );
	screenToLocal( x, y );

	int w, h;
	getPaintSize( w, h );

	if( vertical == false )
	{
		knobPos[0] = knobDragStartPos[0] + ( x - dragStartPos[0] );
		knobPos[1] = knobDragStartPos[1] + ( x - dragStartPos[0] );

		if( knobPos[1] > w )
		{
			knobPos[0] = w - ( knobPos[1] - knobPos[0] );
			knobPos[1] = w;
		}
	}
	else
	{
		knobPos[0] = knobDragStartPos[0] + ( y - dragStartPos[1] );
		knobPos[1] = knobDragStartPos[1] + ( y - dragStartPos[1] );

		if( knobPos[1] > h )
		{
			knobPos[0] = h - ( knobPos[1] - knobPos[0] );
			knobPos[1] = h;
		}
	}

	if( knobPos[0] < 0 )
	{
		knobPos[1] = knobPos[1] - knobPos[0];
		knobPos[0] = 0;
	}

	recomputeValueFromNobPos();
	repaint();
	fireIntChangeSignal();
}

void Slider::privateMousePressed( MouseCode code, Panel *p )
{
	int mx, my;
	getApp()->getCursorPos( mx, my );
	screenToLocal( mx, my );

	if( vertical == false )
	{
		if( mx < knobPos[0] || mx >= knobPos[1] )
			return;
	}
	else
	{
		if( my < knobPos[0] || my >= knobPos[1] )
			return;
	}

	dragging = true;
	getApp()->setMouseCapture( this );
	knobDragStartPos[0] = knobPos[0];
	knobDragStartPos[1] = knobPos[1];
	dragStartPos[0] = mx;
	dragStartPos[1] = my;
}

void Slider::privateMouseReleased( MouseCode code, Panel *p )
{
	dragging = false;
	getApp()->setMouseCapture( nullptr );
}

void Slider::fireIntChangeSignal()
{
	for( int i = 0; i < intChangeSignals.getCount(); i++ )
		intChangeSignals[i]->intChanged( getValue(), this );
}

void Slider::paintBackground()
{
	int w, h;
	getPaintSize( w, h );

	if( !vertical )
	{
		drawSetColor( Scheme::SC_SECONDARY3 );
		drawFilledRect( 0, 0, w, h );
		drawSetColor( Scheme::SC_BLACK );
		drawOutlinedRect( 0, 0, w, h );
		drawSetColor( Scheme::SC_PRIMARY2 );
		drawFilledRect( knobPos[0], 0, knobPos[1], h );
		drawSetColor( Scheme::SC_BLACK );
		drawOutlinedRect( knobPos[0], 0, knobPos[1], h );
	}
	else
	{
		drawSetColor( Scheme::SC_SECONDARY1 );
		drawFilledRect( 0,     0,     w, 1 );
		drawFilledRect( 0,     h - 1, w, h );
		drawFilledRect( 0,     1,     1, h - 1 );
		drawFilledRect( w - 1, 1,     w, h - 1 );
		drawSetColor( Scheme::SC_SECONDARY2 );
		drawFilledRect( 1, 1,          w - 1, 2 );
		drawFilledRect( 1, 2,          3,     h - 1 );
		drawFilledRect( 2, knobPos[1], w - 1, knobPos[1] + 1 );
		drawSetColor( Scheme::SC_SECONDARY3 );
		drawFilledRect( 2, 2, w - 1, h - 1 );
		drawSetColor( Scheme::SC_PRIMARY1 );
		drawFilledRect( 0,     knobPos[0],     w, knobPos[0] + 1 );
		drawFilledRect( 0,     knobPos[1],     w, knobPos[1] + 1 ) ;
		drawFilledRect( 0,     knobPos[0] + 1, 1, knobPos[1] );
		drawFilledRect( w - 1, knobPos[0] + 1, w, knobPos[1] );
		drawSetColor( Scheme::SC_PRIMARY3 );
		drawFilledRect( 1, knobPos[0] + 1, w - 1, knobPos[0] + 2 );
		drawFilledRect( 1, knobPos[0] + 2, 2,     knobPos[1] );
		drawSetColor( Scheme::SC_PRIMARY2 );
		drawFilledRect( 2, knobPos[0] + 2, w - 1, knobPos[1] );
	}
}

class DefaultScrollBarIntChangeSignal : public IntChangeSignal
{
	ScrollBar *scrollBar;
public:
	DefaultScrollBarIntChangeSignal( ScrollBar *sb ) : scrollBar( sb ) {}
	virtual void intChanged( int val, Panel *p ) override
	{
		scrollBar->fireIntChangeSignal();
	}
};

class DefaultButtonSignal : public ActionSignal
{
	ScrollBar *scrollBar;
	int buttonIndex;
public:
	DefaultButtonSignal( ScrollBar *sb, int index ) : scrollBar( sb ), buttonIndex( index ) {}
	virtual void actionPerformed( Panel *p ) override
	{
		scrollBar->doButtonPressed( buttonIndex );
	}
};

ScrollBar::ScrollBar( int x, int y, int w, int h, bool vertical ) : Panel( x, y, w, h )
{
	slider = nullptr;
	buttons[0] = buttons[1] = nullptr;

	if( vertical )
	{
		setSlider( new Slider( 0, w, w, h - w * 2, true ));
		setButton( new Button( "", 0, 0, w, w ), 0 );
		setButton( new Button( "", 0, h - w, w, w ), 1 );
	}
	else
	{
		setSlider( new Slider( h, 0, w - h * 2, h, false ));
		setButton( new Button( "", 0, 0, h, h ), 0 );
		setButton( new Button( "", w - h, 0, h, h ), 1 );
	}

	setPaintBorderEnabled( true );
	setPaintBackgroundEnabled( true );
	setButtonPressedScrollValue( 15 );
	setPaintEnabled( true );

	validate();
}

void ScrollBar::setValue( int value )
{
	slider->setValue( value );
}

int ScrollBar::getValue()
{
	return slider->getValue();
}

void ScrollBar::addIntChangeSignal( IntChangeSignal *ics )
{
	intChangeSignals.putElement( ics );
	slider->addIntChangeSignal( new DefaultScrollBarIntChangeSignal( this ));
}

void ScrollBar::setRange( int imin, int imax )
{
	slider->setRange( imin, imax );
}

void ScrollBar::setRangeWindow( int rangeWindow )
{
	slider->setRangeWindow( rangeWindow );
}

void ScrollBar::setRangeWindowEnabled( bool enable )
{
	slider->setRangeWindowEnabled( enable );
}

void ScrollBar::setSize( int w, int h )
{
	Panel::setSize( w, h );

	if( !slider || !buttons[0] || !buttons[1] )
		return;

	getPaintSize( w, h );

	if( slider->isVertical( ))
	{
		slider->setBounds( 0, w, w, h - w * 2 );
		buttons[0]->setBounds( 0, 0, w, w );
		buttons[1]->setBounds( 0, h - w, w, w );
	}
	else
	{
		slider->setBounds( h, 0, w - h * 2, h );
		buttons[0]->setBounds( 0, 0, h, h );
		buttons[1]->setBounds( w - h, 0, h, h );
	}
}

bool ScrollBar::isVertical()
{
	return slider->isVertical();
}

bool ScrollBar::hasFullRange()
{
	return slider->hasFullRange();
}

void ScrollBar::setButton( Button *b, int i )
{
	if( buttons[i] )
		removeChild( buttons[i] );

	buttons[i] = b;
	addChild( b );
	b->addActionSignal( new DefaultButtonSignal( this, i ));
	validate();
}

Button *ScrollBar::getButton( int i )
{
	return buttons[i];
}

void ScrollBar::setSlider( Slider *s )
{
	if( slider )
		removeChild( slider );

	slider = s;
	addChild( s );
	s->addIntChangeSignal( new DefaultScrollBarIntChangeSignal( this ));
	validate();
}

Slider *ScrollBar::getSlider()
{
	return slider;
}

void ScrollBar::doButtonPressed( int i )
{
	slider->setValue( slider->getValue() + ( buttonPressedScrollValue * ( i == 0 ? -1 : 1 )));
}

void ScrollBar::setButtonPressedScrollValue( int i )
{
	buttonPressedScrollValue = i;
}

void ScrollBar::validate()
{
	if( slider )
	{
		int buttonOffset = 0;
		bool vertical = slider->isVertical();

		for( int i = 0; i < 2; i++ )
		{
			if( buttons[i] == nullptr || !buttons[i]->isVisible( ))
				continue;

			if( vertical )
				buttonOffset += buttons[i]->getTall();
			else
				buttonOffset += buttons[i]->getWide();
		}

		slider->setButtonOffset( buttonOffset );
	}

	int w, h;
	getSize( w, h );
	setSize( w, h );
}

void ScrollBar::fireIntChangeSignal()
{
	for( int i = 0; i < intChangeSignals.getCount(); i++ )
		intChangeSignals[i]->intChanged( slider->getValue(), this );
}

void ScrollBar::performLayout()
{

}

class ChangeHandler : public IntChangeSignal
{
	ScrollPanel *scrollPanel;

public:
	ChangeHandler( ScrollPanel *sp ) : scrollPanel( sp ) {}

	void intChanged( int value, Panel *p ) override
	{
		scrollPanel->recomputeScroll();
	}
};

ScrollPanel::ScrollPanel( int x, int y, int w, int h ) : Panel( x, y, w, h )
{
	setPaintBorderEnabled( true );
	setPaintBackgroundEnabled( false );
	setPaintEnabled( false );

	clientClip = new Panel( 0, 0, w - 16, h - 16 );
	clientClip->setParent( this );
	clientClip->setBgColor( Color( 0, 128, 0, 0 ));
	clientClip->setPaintBorderEnabled( true );
	clientClip->setPaintBackgroundEnabled( false );
	clientClip->setPaintEnabled( false );

	client = new Panel( 0, 0, w * 2, h * 2 );
	client->setParent( clientClip );
	client->setPaintBorderEnabled( true );
	client->setPaintBackgroundEnabled( false );
	client->setPaintEnabled( false );

	horizontalScrollBar = new ScrollBar( 0, h - 16, w - 16, 16, false );
	horizontalScrollBar->setParent( this );
	horizontalScrollBar->addIntChangeSignal( new ChangeHandler( this ));
	horizontalScrollBar->setVisible( false );

	verticalScrollBar = new ScrollBar( w - 16, 0, 16, h - 16, true );
	verticalScrollBar->setParent( this );
	verticalScrollBar->addIntChangeSignal( new ChangeHandler( this ));
	verticalScrollBar->setVisible( false );

	autoVisible[0] = autoVisible[1] = true;

	validate();
}

void ScrollPanel::setSize( int w, int h )
{
	Panel::setSize( w, h );
	getPaintSize( w, h );

	if( autoVisible[1] )
		verticalScrollBar->setVisible( !verticalScrollBar->hasFullRange( ));

	if( verticalScrollBar->isVisible( ))
		w -= verticalScrollBar->getWide();

	if( autoVisible[0] )
		horizontalScrollBar->setVisible( !horizontalScrollBar->hasFullRange( ));

	if( horizontalScrollBar->isVisible( ))
		h -= horizontalScrollBar->getTall();

	verticalScrollBar->setBounds( w, 0, verticalScrollBar->getWide(), h );
	horizontalScrollBar->setBounds( 0, h, w, horizontalScrollBar->getTall());
	clientClip->setSize( w, h );
	recomputeClientSize();
	repaint();
}

void ScrollPanel::setScrollBarVisible( bool h, bool v )
{
	horizontalScrollBar->setVisible( h );
	verticalScrollBar->setVisible( v );

	validate();
}

void ScrollPanel::setScrollBarAutoVisible( bool h, bool v )
{
	autoVisible[0] = h;
	autoVisible[1] = v;
	validate();
}

Panel *ScrollPanel::getClient()
{
	return client;
}

Panel *ScrollPanel::getClientClip()
{
	return clientClip;
}

void ScrollPanel::setScrollValue( int h, int v )
{
	horizontalScrollBar->setValue( h );
	verticalScrollBar->setValue( v );
	recomputeScroll();
}

void ScrollPanel::getScrollValue( int &h, int &v )
{
	h = horizontalScrollBar->getValue();
	v = verticalScrollBar->getValue();
}

void ScrollPanel::recomputeClientSize()
{
	int total_w = 0, total_h = 0;

	for( int i = 0; i < client->getChildCount(); i++ )
	{
		Panel *p = client->getChild( i );

		if( !p->isVisible() )
			continue;

		int x, y, w, h;
		p->getPos( x, y );
		p->getVirtualSize( w, h );

		x += w;
		y += h;

		if( total_w < x )
			total_w = x;

		if( total_h < y )
			total_h = y;
	}

	client->setSize( total_w, total_h );

	horizontalScrollBar->setRange( 0, client->getWide() - clientClip->getWide( ));
	horizontalScrollBar->setRangeWindow( client->getWide( ));

	verticalScrollBar->setRange( 0, client->getTall() - clientClip->getTall( ));
	verticalScrollBar->setRangeWindow( client->getTall( ));
}

ScrollBar *ScrollPanel::getHorizontalScrollBar()
{
	return horizontalScrollBar;
}

ScrollBar *ScrollPanel::getVerticalScrollBar()
{
	return verticalScrollBar;
}

void ScrollPanel::validate()
{
	horizontalScrollBar->setRangeWindowEnabled( true );
	verticalScrollBar->setRangeWindowEnabled( true );

	int w, h;
	getSize( w, h );
	setSize( w, h );
	setSize( w, h );
	setSize( w, h );
	setSize( w, h ); // wtf?
}

void ScrollPanel::recomputeScroll()
{
	int x, y;
	getScrollValue( x, y );
	client->setPos( -x, -y );
	clientClip->repaint();
}

}
