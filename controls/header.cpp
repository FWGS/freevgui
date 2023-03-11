// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "app.h"
#include "input.h"
#include "header.h"

namespace vgui
{

class HeaderPanelSignal : public InputSignal
{
public:
	HeaderPanelSignal( HeaderPanel *headerPanel ) : _headerPanel( headerPanel ) {}

	virtual void cursorMoved( int x, int y, Panel *p ) override
	{
		_headerPanel->privateCursorMoved( x, y, p );
	}
	virtual void cursorEntered( Panel * ) override {}
	virtual void cursorExited( Panel * ) override {}
	virtual void mousePressed( MouseCode code, Panel *p ) override
	{
		_headerPanel->privateMousePressed( code, p );
	}
	virtual void mouseDoublePressed( MouseCode, Panel * ) override {}
	virtual void mouseReleased( MouseCode code, Panel *p ) override
	{
		_headerPanel->privateMouseReleased( code, p );
	}
	virtual void mouseWheeled( int, Panel * ) override {}
	virtual void keyPressed( KeyCode, Panel * ) override {}
	virtual void keyTyped( KeyCode, Panel * ) override {}
	virtual void keyReleased( KeyCode, Panel * ) override {}
	virtual void keyFocusTicked( Panel * ) override {}
private:
	HeaderPanel *_headerPanel;
};

void HeaderPanel::performLayout()
{
	int x_left = 0, w, h;
	getPaintSize( w, h );

	_sectionLayer->setBounds( 0, 0, w, h );

	for( int i = 0; i < _sectionPanelDar.getCount(); i++ )
	{
		int x, y, x_right;

		Panel *slider = _sliderPanelDar[i];
		slider->getPos( x, y );

		x_right = x + _sliderWide / 2;

		Panel *panel = _sectionPanelDar[i];
		panel->setBounds( x_left, 0, x_right - x_left, h );

		x_left = x_right;
	}
}

HeaderPanel::HeaderPanel( int x, int y, int w, int h ) :
	Panel( x, y, w, h ),
	_sectionLayer( new Panel( 0, 0, w, h )),
	_sliderWide( 11 ),
	_dragging( false )
{
	_sectionLayer->setPaintBorderEnabled( false );
	_sectionLayer->setPaintBackgroundEnabled( false );
	_sectionLayer->setPaintEnabled( false );
	_sectionLayer->setParent( this );
}

void HeaderPanel::addSectionPanel( Panel *p )
{
	invalidateLayout( true );

	int x = 0, y = 0, w = 0, h = 0;

	for( int i = 0; i < _sectionPanelDar.getCount(); i++ )
	{
		_sectionPanelDar[i]->getBounds( x, y, w, h );
		x += w + _sliderWide;
	}
	_sectionPanelDar.addElement( p );
	p->setPos( x, 0 );
	p->setParent( _sectionLayer );
	p->setBounds( x, y, w, h );

	getPaintSize( w, h );

	Panel *slider = new Panel( 0, 0, _sliderWide, h );
	slider->setPaintBorderEnabled( false );
	slider->setPaintBackgroundEnabled( false );
	slider->setPaintEnabled( false );
	slider->setPos( w + x, 0 );
	slider->addInputSignal( new HeaderPanelSignal( this ));
	slider->setCursor( getApp()->getScheme()->getCursor( (Scheme::SchemeCursor)Cursor::DefaultCursor::DC_SIZEWE ));
	slider->setParent( this );
	_sliderPanelDar.addElement( slider );

	invalidateLayout( false );
	fireChangeSignal();
	repaint();
}

void HeaderPanel::setSliderPos( int i, int pos )
{
	_sliderPanelDar[i]->setPos( pos - _sliderWide / 2, 0 );
	invalidateLayout( false );
	fireChangeSignal();
	repaint();
}

int HeaderPanel::getSectionCount()
{
	return _sectionPanelDar.getCount();
}

void HeaderPanel::getSectionExtents( int i, int &x_left, int &x_right )
{
	int x, y, w, h;
	_sectionPanelDar[i]->getBounds( x, y, w, h );
	x_left = x;
	x_right = x + w;
}

void HeaderPanel::addChangeSignal( ChangeSignal *s )
{
	_changeSignalDar.addElement( s );
}

void HeaderPanel::fireChangeSignal()
{
	invalidateLayout( true );
	for( int i = 0; i < _changeSignalDar.getCount(); i++ )
		_changeSignalDar[i]->valueChanged( this );
}

void HeaderPanel::privateCursorMoved( int x, int y, Panel *p )
{
	if( _dragging )
	{
		getApp()->getCursorPos( x, y );
		screenToLocal( x, y );
		setSliderPos( _dragSliderIndex, x + _dragSliderStartPos - _dragSliderStartX );
		invalidateLayout( false );
		repaint();
	}
}

void HeaderPanel::privateMousePressed( MouseCode code, Panel *p )
{
	int mx, my;

	getApp()->getCursorPos( mx, my );
	screenToLocal( mx, my );

	for( int i = 0; i < _sliderPanelDar.getCount(); i++ )
	{
		Panel *slider = _sliderPanelDar[i];
		if( slider == p )
		{
			int x, y;

			p->getPos( x, y );
			_dragging = true;
			_dragSliderIndex = i;
			_dragSliderStartPos = _sliderWide / 2 + x;
			_dragSliderStartX = mx;
			setAsMouseCapture( true );
			break;
		}
	}
}

void HeaderPanel::privateMouseReleased( MouseCode code, Panel *p )
{
	_dragging = false;
	setAsMouseCapture( false );
}

}
