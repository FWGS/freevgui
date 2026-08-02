// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "app.h"
#include "vgui_internal.h"
#include "signals.h"
#include "scroll.h"

namespace vgui
{
class FooDefaultSliderSignal : public InputSignal
{
	Slider *_slider;
public:
	FooDefaultSliderSignal( Slider *slider ) : _slider( slider ) {}
	virtual void cursorMoved( int x, int y, Panel *p ) override
	{
		_slider->privateCursorMoved( x, y, p );
	}
	virtual void cursorEntered( Panel * ) override {}
	virtual void cursorExited( Panel * ) override {}
	virtual void mousePressed( MouseCode code, Panel *p ) override
	{
		_slider->privateMousePressed( code, p );
	}
	virtual void mouseDoublePressed( MouseCode, Panel * ) override {}
	virtual void mouseReleased( MouseCode code, Panel *p ) override
	{
		_slider->privateMouseReleased( code, p );
	}
	virtual void mouseWheeled( int, Panel * ) override {}
	virtual void keyPressed( KeyCode, Panel * ) override {}
	virtual void keyTyped( KeyCode, Panel * ) override {}
	virtual void keyReleased( KeyCode, Panel * ) override {}
	virtual void keyFocusTicked( Panel * ) override {}
};


Slider::Slider( int x, int y, int w, int h, bool vertical ) :
	Panel( x, y, w, h ),
	_vertical( vertical ),
	_dragging( false ),
	_value( 0 ),
	_rangeWindow( 0 ),
	_rangeWindowEnabled( false ),
	_buttonOffset( 0 )
{
	Vector2Set( _range, 0, 299 );
	recomputeNobPosFromValue();
	addInputSignal( new FooDefaultSliderSignal( this ));
}

void Slider::setValue(int value)
{
	int orig_value = _value;

	value = bound( _range[0], value, _range[1] );

	_value = value;
	recomputeNobPosFromValue();

	if( _value != orig_value )
		fireIntChangeSignal();
}

int Slider::getValue()
{
	return _value;
}

bool Slider::isVertical()
{
	return _vertical;
}

void Slider::addIntChangeSignal(IntChangeSignal *s)
{
	_intChangeSignalDar.addElement( s );
}

void Slider::setRange(int imin, int imax)
{
	if( imax < imin )
		imax = imin;

	Vector2Set( _range, imin, imax );
}

void Slider::getRange(int &imin, int &imax)
{
	imin = _range[0];
	imax = _range[1];
}

void Slider::setRangeWindow(int rangeWindow)
{
	_rangeWindow = rangeWindow;
}

void Slider::setRangeWindowEnabled(bool enable)
{
	_rangeWindowEnabled = enable;
}

void Slider::setSize(int w, int h)
{
	Panel::setSize( w, h );
	recomputeNobPosFromValue();
}

void Slider::getNobPos(int &imin, int &imax)
{
	imin = _nobPos[0];
	imax = _nobPos[1];
}

bool Slider::hasFullRange()
{
	int w, h;
	getPaintSize( w, h );

	float range = _range[1] - _range[0];
	float rangewindow = _rangeWindowEnabled ? _rangeWindow : range;

	if( rangewindow < 0.0f )
		return false;

	float size = _vertical ? h : w;
	return rangewindow <= size + _buttonOffset;
}

void Slider::setButtonOffset(int off)
{
	_buttonOffset = off;
}

void Slider::recomputeNobPosFromValue()
{
	int w, h;
	getPaintSize( w, h );

	float range = _range[1] - _range[0];
	float value = _value - _range[0];
	float rangewindow = range;

	if( _rangeWindow < 0.0f && _rangeWindowEnabled )
	{
		repaint();
		return;
	}

	if( _rangeWindow >= 0.0f && _rangeWindowEnabled )
		rangewindow = _rangeWindow;

	if( rangewindow > 0.0f )
	{
		float size = _vertical ? h : w;
		float nobSize = size / rangewindow * size;

		_nobPos[0] = ( size - nobSize ) * ( value / size);
		_nobPos[1] = _nobPos[0] + nobSize;

		if( _nobPos[1] > size )
		{
			_nobPos[0] = h - nobSize;
			_nobPos[1] = h;
		}
	}
}

void Slider::recomputeValueFromNobPos()
{
	int w, h;

	getPaintSize( w, h );

	float range = _range[1] - _range[0];
	float value = _value - _range[0];
	float rangewindow = range;

	if( _rangeWindow < 0.0f && _rangeWindowEnabled )
	{
		_value = bound( _range[0], (int)( _range[0] + value + 0.5f ), _range[1] );
		return;
	}

	if( _rangeWindow >= 0.0f && _rangeWindowEnabled )
		rangewindow = _rangeWindow;

	if( rangewindow > 0 )
	{
		float size = _vertical ? h : w;
		value = _nobPos[0] / ( size - ( size / rangewindow ) * size ) * range;
	}

	_value = bound( _range[0], (int)( _range[0] + value + 0.5f ), _range[1] );
}

void Slider::privateCursorMoved( int x, int y, Panel *p )
{
	if( _dragging == false )
		return;

	getApp()->getCursorPos( x, y );
	screenToLocal( x, y );

	int w, h;
	getPaintSize( w, h );

	if( _vertical == false )
	{
		_nobPos[0] = _nobDragStartPos[0] + ( x - _dragStartPos[1] );
		_nobPos[1] = _nobDragStartPos[1] + ( x - _dragStartPos[1] );

		if( _nobPos[1] > w )
		{
			_nobPos[0] = w - ( _nobPos[1] - _nobPos[0] );
			_nobPos[1] = w;
		}
	}
	else
	{
		_nobPos[0] = _nobDragStartPos[0] + ( y - _dragStartPos[0] );
		_nobPos[1] = _nobDragStartPos[1] + ( y - _dragStartPos[0] );

		if( _nobPos[1] > h )
		{
			_nobPos[0] = h - ( _nobPos[1] - _nobPos[0] );
			_nobPos[1] = h;
		}
	}

	if( _nobPos[0] < 0 )
	{
		_nobPos[1] = _nobPos[1] - _nobPos[0];
		_nobPos[0] = 0;
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

	if( _vertical == false )
	{
		if( mx < _nobPos[0] || mx >= _nobPos[1] )
			return;
	}
	else
	{
		if( my < _nobPos[0] || my >= _nobPos[1] )
			return;
	}

	_dragging = true;
	getApp()->setMouseCapture( this );
	Vector2Copy( _nobPos, _nobDragStartPos );
	Vector2Set( _dragStartPos, mx, my );
}

void Slider::privateMouseReleased( MouseCode code, Panel *p )
{
	_dragging = false;
	getApp()->setMouseCapture( nullptr );
}

void Slider::fireIntChangeSignal()
{
	for( int i = 0; i < _intChangeSignalDar.getCount(); i++ )
		_intChangeSignalDar[i]->intChanged( getValue(), this );
}

void Slider::paintBackground()
{
	int w, h;
	getPaintSize( w, h );

	if( !_vertical )
	{
		drawSetColor( Scheme::SC_SECONDARY3 );
		drawFilledRect( 0, 0, w, h );
		drawSetColor( Scheme::SC_BLACK );
		drawOutlinedRect( 0, 0, w, h );
		drawSetColor( Scheme::SC_PRIMARY2 );
		drawFilledRect( _nobPos[0], 0, _nobPos[1], h );
		drawSetColor( Scheme::SC_BLACK );
		drawOutlinedRect( _nobPos[0], 0, _nobPos[1], h );
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
		drawFilledRect( 2, _nobPos[1], w - 1, _nobPos[1] + 1 );
		drawSetColor( Scheme::SC_SECONDARY3 );
		drawFilledRect( 2, 2, w - 1, h - 1 );
		drawSetColor( Scheme::SC_PRIMARY1 );
		drawFilledRect( 0,     _nobPos[0],     w, _nobPos[0] + 1 );
		drawFilledRect( 0,     _nobPos[1],     w, _nobPos[1] + 1 ) ;
		drawFilledRect( 0,     _nobPos[0] + 1, 1, _nobPos[1] );
		drawFilledRect( w - 1, _nobPos[0] + 1, w, _nobPos[1] );
		drawSetColor( Scheme::SC_PRIMARY3 );
		drawFilledRect( 1, _nobPos[0] + 1, w - 1, _nobPos[0] + 2 );
		drawFilledRect( 1, _nobPos[0] + 2, 2,     _nobPos[1] );
		drawSetColor( Scheme::SC_PRIMARY2 );
		drawFilledRect( 2, _nobPos[0] + 2, w - 1, _nobPos[1] );
	}
}

class FooDefaultScrollBarIntChangeSignal : public IntChangeSignal
{
	ScrollBar *_sb;
public:
	FooDefaultScrollBarIntChangeSignal( ScrollBar *sb ) : _sb( sb ) {}
	virtual void intChanged( int val, Panel *p ) override
	{
		_sb->fireIntChangeSignal();
	}
};

class FooDefaultButtonSignal : public ActionSignal
{
	ScrollBar *_sb;
	int _buttonIndex;
public:
	FooDefaultButtonSignal( ScrollBar *sb, int index ) : _sb( sb ), _buttonIndex( index ) {}
	virtual void actionPerformed( Panel *p ) override
	{
		_sb->doButtonPressed( _buttonIndex );
	}
};

ScrollBar::ScrollBar( int x, int y, int w, int h, bool vertical ) : Panel( x, y, w, h )
{
	_slider = nullptr;
	_button[0] = _button[1] = nullptr;

	if( vertical )
	{
		setSlider( new Slider( 0, w - 1, w, h - w * 2, true ));
		setButton( new Button( "", 0, 0, w, w ), 0 );
		setButton( new Button( "", 0, h - w, w, w ), 1 );
	}
	else
	{
		setSlider( new Slider( h, 0, w - h * 2, h, false ));
		setButton( new Button( "", 0, 0, h + 1, h + 1 ), 0 );
		setButton( new Button( "", w - h, 0, h + 1, h + 1 ), 1 );
	}

	setPaintBorderEnabled( true );
	setPaintBackgroundEnabled( true );
	setButtonPressedScrollValue( 15 );
	setPaintEnabled( true );

	validate();
}

void ScrollBar::setValue( int value )
{
	_slider->setValue( value );
}

int ScrollBar::getValue()
{
	return _slider->getValue();
}

void ScrollBar::addIntChangeSignal( IntChangeSignal *ics )
{
	_intChangeSignalDar.putElement( ics );
	_slider->addIntChangeSignal( new FooDefaultScrollBarIntChangeSignal( this ));
}

void ScrollBar::setRange( int imin, int imax )
{
	_slider->setRange( imin, imax );
}

void ScrollBar::setRangeWindow( int rangeWindow )
{
	_slider->setRangeWindow( rangeWindow );
}

void ScrollBar::setRangeWindowEnabled( bool enable )
{
	_slider->setRangeWindowEnabled( enable );
}

void ScrollBar::setSize( int w, int h )
{
	Panel::setSize( w, h );

	if( !_slider || !_button[0] || !_button[1] )
		return;

	getPaintSize( w, h );

	if( _slider->isVertical( ))
	{
		_slider->setBounds( 0, w - 1, w, h - w * 2 );
		_button[0]->setBounds( 0, 0, w, w );
		_button[1]->setBounds( 0, h - w, w, w );
	}
	else
	{
		_slider->setBounds( h, 0, w - h * 2, h );
		_button[0]->setBounds( 0, 0, h + 1, h + 1 );
		_button[1]->setBounds( w - h, 0, h + 1, h + 1 );
	}
}

bool ScrollBar::isVertical()
{
	return _slider->isVertical();
}

bool ScrollBar::hasFullRange()
{
	return _slider->hasFullRange();
}

void ScrollBar::setButton( Button *b, int i )
{
	if( _button[i] )
		removeChild( _button[i] );

	_button[i] = b;
	addChild( b );
	b->addActionSignal( new FooDefaultButtonSignal( this, i ));
	validate();
}

Button *ScrollBar::getButton( int i )
{
	return _button[i];
}

void ScrollBar::setSlider( Slider *s )
{
	if( _slider )
		removeChild( _slider );

	_slider = s;
	addChild( s );
	s->addIntChangeSignal( new FooDefaultScrollBarIntChangeSignal( this ));
	validate();
}

Slider *ScrollBar::getSlider()
{
	return _slider;
}

void ScrollBar::doButtonPressed( int i )
{
	_slider->setValue( _slider->getValue() + ( _buttonPressedScrollValue * ( i == 0 ? -1 : 1 )));
}

void ScrollBar::setButtonPressedScrollValue( int i )
{
	_buttonPressedScrollValue = i;
}

void ScrollBar::validate()
{
	if( _slider )
	{
		int buttonOffset = 0;
		bool vertical = _slider->isVertical();

		for( int i = 0; i < 2; i++ )
		{
			if( _button[i] == nullptr || !_button[i]->isVisible( ))
				continue;

			if( vertical )
				buttonOffset += _button[i]->getTall();
			else
				buttonOffset += _button[i]->getWide();
		}

		_slider->setButtonOffset( buttonOffset );
	}

	int w, h;
	getSize( w, h );
	setSize( w, h );
}

void ScrollBar::fireIntChangeSignal()
{
	for( int i = 0; i < _intChangeSignalDar.getCount(); i++ )
		_intChangeSignalDar[i]->intChanged( _slider->getValue(), this );
}

void ScrollBar::performLayout()
{

}

class ChangeHandler : public IntChangeSignal
{
	ScrollPanel *_sp;

public:
	ChangeHandler( ScrollPanel *sp ) : _sp( sp ) {}

	void intChanged( int value, Panel *p ) override
	{
		_sp->recomputeScroll();
	}
};

ScrollPanel::ScrollPanel( int x, int y, int w, int h ) : Panel( x, y, w, h )
{
	setPaintBorderEnabled( true );
	setPaintBackgroundEnabled( false );
	setPaintEnabled( false );

	_clientClip = new Panel( 0, 0, w - 16, h - 16 );
	_clientClip->setParent( this );
	_clientClip->setBgColor( Color( 0, 128, 0, 0 ));
	_clientClip->setPaintBorderEnabled( true );
	_clientClip->setPaintBackgroundEnabled( false );
	_clientClip->setPaintEnabled( false );

	_client = new Panel( 0, 0, w * 2, h * 2 );
	_client->setParent( _clientClip );
	_client->setPaintBorderEnabled( true );
	_client->setPaintBackgroundEnabled( false );
	_client->setPaintEnabled( false );

	_horizontalScrollBar = new ScrollBar( 0, h - 16, w - 16, 16, false );
	_horizontalScrollBar->setParent( this );
	_horizontalScrollBar->addIntChangeSignal( new ChangeHandler( this ));
	_horizontalScrollBar->setVisible( false );

	_verticalScrollBar = new ScrollBar( w - 16, 0, 16, h - 16, true );
	_verticalScrollBar->setParent( this );
	_verticalScrollBar->addIntChangeSignal( new ChangeHandler( this ));
	_verticalScrollBar->setVisible( false );

	_autoVisible[0] = _autoVisible[1] = true;

	validate();
}

void ScrollPanel::setSize( int w, int h )
{
	Panel::setSize( w, h );
	getPaintSize( w, h );

	if( _autoVisible[1] )
		_verticalScrollBar->setVisible( !_verticalScrollBar->hasFullRange( ));

	if( _verticalScrollBar->isVisible( ))
		w -= _verticalScrollBar->getWide();

	if( _autoVisible[0] )
		_horizontalScrollBar->setVisible( !_horizontalScrollBar->hasFullRange( ));

	if( _horizontalScrollBar->isVisible( ))
		h -= _horizontalScrollBar->getTall();

	_verticalScrollBar->setBounds( w, 0, _verticalScrollBar->getWide(), h );
	_horizontalScrollBar->setBounds( 0, h, w, _horizontalScrollBar->getTall());
	_clientClip->setSize( w, h );
	recomputeClientSize();
	repaint();
}

void ScrollPanel::setScrollBarVisible( bool h, bool v )
{
	_horizontalScrollBar->setVisible( h );
	_verticalScrollBar->setVisible( v );

	validate();
}

void ScrollPanel::setScrollBarAutoVisible( bool h, bool v )
{
	Vector2Set( _autoVisible, h, v );
	validate();
}

Panel *ScrollPanel::getClient()
{
	return _client;
}

Panel *ScrollPanel::getClientClip()
{
	return _clientClip;
}

void ScrollPanel::setScrollValue( int h, int v )
{
	_horizontalScrollBar->setValue( h );
	_verticalScrollBar->setValue( v );
	recomputeScroll();
}

void ScrollPanel::getScrollValue( int &h, int &v )
{
	h = _horizontalScrollBar->getValue();
	v = _verticalScrollBar->getValue();
}

void ScrollPanel::recomputeClientSize()
{
	int total_w = 0, total_h = 0;

	for( int i = 0; i < _client->getChildCount(); i++ )
	{
		Panel *p = _client->getChild( i );

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

	_client->setSize( total_w, total_h );

	_horizontalScrollBar->setRange( 0, _client->getWide() - _clientClip->getWide( ));
	_horizontalScrollBar->setRangeWindow( _client->getWide( ));

	_verticalScrollBar->setRange( 0, _client->getTall() - _clientClip->getTall( ));
	_verticalScrollBar->setRangeWindow( _client->getTall( ));
}

ScrollBar *ScrollPanel::getHorizontalScrollBar()
{
	return _horizontalScrollBar;
}

ScrollBar *ScrollPanel::getVerticalScrollBar()
{
	return _verticalScrollBar;
}

void ScrollPanel::validate()
{
	_horizontalScrollBar->setRangeWindowEnabled( true );
	_verticalScrollBar->setRangeWindowEnabled( true );

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
	_client->setPos( -x, -y );
	_clientClip->repaint();
}

}
