// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "app.h"
#include "input.h"
#include "header.h"

namespace vgui
{
class HeaderPanelSignal : public InputSignalAdapter
{
	HeaderPanel *hp;
public:
	HeaderPanelSignal( HeaderPanel *hp ) : hp( hp ) {}

	virtual void cursorMoved( int x, int y, Panel *p ) override
	{
		hp->privateCursorMoved( x, y, p );
	}
	virtual void mousePressed( MouseCode code, Panel *p ) override
	{
		hp->privateMousePressed( code, p );
	}
	virtual void mouseReleased( MouseCode code, Panel *p ) override
	{
		hp->privateMouseReleased( code, p );
	}
};

void HeaderPanel::performLayout()
{
	int x_left = 0, w, h;
	getPaintSize( w, h );

	sectionLayer->setBounds( 0, 0, w, h );

	for( int i = 0; i < sectionPanels.getCount(); i++ )
	{
		int x, y, x_right;

		Panel *slider = sliderPanels[i];
		slider->getPos( x, y );

		x_right = x + sliderWide / 2;

		Panel *panel = sectionPanels[i];
		panel->setBounds( x_left, 0, x_right - x_left, h );

		x_left = x_right;
	}
}

HeaderPanel::HeaderPanel( int x, int y, int w, int h ) : Panel( x, y, w, h ),
	sectionLayer( new Panel( 0, 0, w, h )),
	sliderWide( 11 ),
	dragging( false )
{
	sectionLayer->setPaintBorderEnabled( false );
	sectionLayer->setPaintBackgroundEnabled( false );
	sectionLayer->setPaintEnabled( false );
	sectionLayer->setParent( this );
}

void HeaderPanel::addSectionPanel( Panel *p )
{
	invalidateLayout( true );

	int x = 0, y = 0, w = 0, h = 0;

	for( auto section : sectionPanels )
	{
		section->getBounds( x, y, w, h );

		x += w + sliderWide;
	}
	sectionPanels.addElement( p );
	p->setPos( x, 0 );
	p->setParent( sectionLayer );
	p->setBounds( x, y, w, h );

	getPaintSize( w, h );

	Panel *slider = new Panel( 0, 0, sliderWide, h );
	slider->setPaintBorderEnabled( false );
	slider->setPaintBackgroundEnabled( false );
	slider->setPaintEnabled( false );
	slider->setPos( w + x, 0 );
	slider->addInputSignal( new HeaderPanelSignal( this ));
	slider->setCursor( getApp()->getScheme()->getCursor( (Scheme::SchemeCursor)Cursor::DefaultCursor::DC_SIZEWE ));
	slider->setParent( this );
	sliderPanels.addElement( slider );

	invalidateLayout( false );
	fireChangeSignal();
	repaint();
}

void HeaderPanel::setSliderPos( int i, int pos )
{
	sliderPanels[i]->setPos( pos - sliderWide / 2, 0 );
	invalidateLayout( false );
	fireChangeSignal();
	repaint();
}

int HeaderPanel::getSectionCount()
{
	return sectionPanels.getCount();
}

void HeaderPanel::getSectionExtents( int i, int &x_left, int &x_right )
{
	int x, y, w, h;
	sectionPanels[i]->getBounds( x, y, w, h );
	x_left = x;
	x_right = x + w;
}

void HeaderPanel::addChangeSignal( ChangeSignal *s )
{
	changeSignals.putElement( s );
}

void HeaderPanel::fireChangeSignal()
{
	invalidateLayout( true );
	for( auto signal : changeSignals )
		signal->valueChanged( this );
}

void HeaderPanel::privateCursorMoved( int x, int y, Panel *p )
{
	if( !dragging )
		return;

	getApp()->getCursorPos( x, y );
	screenToLocal( x, y );
	setSliderPos( dragSliderIndex, x + dragSliderStartPos - dragSliderStartX );
	invalidateLayout( false );
	repaint();
}

void HeaderPanel::privateMousePressed( MouseCode code, Panel *p )
{
	int mx, my;

	getApp()->getCursorPos( mx, my );
	screenToLocal( mx, my );

	for( int i = 0; i < sliderPanels.getCount(); i++ )
	{
		Panel *slider = sliderPanels[i];

		if( slider != p )
			continue;

		int x, y;

		p->getPos( x, y );
		dragging = true;
		dragSliderIndex = i;
		dragSliderStartPos = sliderWide / 2 + x;
		dragSliderStartX = mx;
		slider->setAsMouseCapture( true );
		break;
	}
}

void HeaderPanel::privateMouseReleased( MouseCode code, Panel *p )
{
	dragging = false;
	p->setAsMouseCapture( false );
}
}
