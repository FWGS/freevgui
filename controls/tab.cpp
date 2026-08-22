// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/tab.h"

using namespace vgui;

namespace // private stuff
{
// a tab button: a ToggleButton that listens to its own action signal and
// tells the owning TabPanel to switch pages when clicked
class TabButton : public ToggleButton, public ActionSignal
{
	TabPanel *tp;

public:
	TabButton( TabPanel *tp, const char *text, int x, int y ) : ToggleButton( text, x, y ), tp( tp )
	{
		addActionSignal( this );
	}

	virtual bool isWithin( int x, int y ) override
	{
		int w, h;

		screenToLocal( x, y );
		getSize( w, h );
		return x >= 0 && x < w && y >= 0 && y < h;
	}

	virtual void actionPerformed( Panel * ) override
	{
		tp->setSelectedTab( this );
	}

	virtual void paintBackground() override
	{
		int wide, tall;

		getPaintSize( wide, tall );

		if( isSelected( ))
		{
			// light face, bottom left open so the tab merges into the client area
			drawSetColor( Scheme::SC_SECONDARY3 );
			drawFilledRect( 0, 0, wide, tall );
			drawSetColor( Scheme::SC_SECONDARY1 );
			drawFilledRect( 0, 0, wide, 1 );
			drawFilledRect( 0, 1, 1, tall - 1 );
			drawFilledRect( wide - 1, 1, wide, tall - 1 );
			drawSetColor( Scheme::SC_WHITE );
			drawFilledRect( 1, 1, wide - 1, 2 );
			drawFilledRect( 1, 2, 2, tall );
			drawFilledRect( 0, tall - 1, 1, tall );
			drawFilledRect( wide - 1, tall - 1, wide, tall );
		}
		else
		{
			// darker face, closed with a white baseline along the bottom
			drawSetColor( Scheme::SC_SECONDARY2 );
			drawFilledRect( 0, 0, wide, tall );
			drawSetColor( Scheme::SC_SECONDARY1 );
			drawFilledRect( 0, 0, wide, 1 );
			drawFilledRect( 0, 1, 1, tall - 1 );
			drawFilledRect( wide - 1, 1, wide, tall - 1 );
			drawSetColor( Scheme::SC_SECONDARY3 );
			drawFilledRect( 1, 1, wide - 1, 2 );
			drawFilledRect( 1, 2, 2, tall );
			drawFilledRect( 0, tall - 1, 1, tall );
			drawFilledRect( wide - 1, tall - 1, wide, tall );
			drawSetColor( Scheme::SC_WHITE );
			drawFilledRect( 0, tall - 1, wide, tall );
		}
	}
};

class ClientAreaBorder : public Border
{
protected:
	virtual void paint( Panel *p ) override
	{
		int wide, tall;

		p->getSize( wide, tall );
		drawSetColor( Scheme::SC_BLACK );
		drawFilledRect( 0, 0, wide - 1, 1 );
		drawFilledRect( 0, 1, 1, tall - 1 );
		drawSetColor( Scheme::SC_SECONDARY1 );
		drawFilledRect( 0, tall - 1, wide, tall );
		drawFilledRect( wide - 1, 0, wide, tall - 1 );
	}
};

class TabAreaBorder : public Border
{
protected:
	virtual void paint( Panel *p ) override
	{
		int wide, tall;

		p->getSize( wide, tall );
		drawSetColor( Scheme::SC_WHITE );
		drawFilledRect( 0, tall - 1, wide, tall );
	}
};
}

TabPanel::TabPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall ),
	tabPlacement( TOP ),
	tabArea( new Panel( 5, 5, wide, 5 )), clientArea( new Panel( 5, 5, wide - 10, tall - 10 )),
	selectedTab( nullptr ), selectedPanel( nullptr ),
	buttonGroup( new ButtonGroup())
{
	clientArea->setParent( this );
	clientArea->setBorder( new ClientAreaBorder());
	tabArea->setParent( this );
	tabArea->setBorder( new TabAreaBorder());
}

Panel *TabPanel::addTab( const char *text )
{
	TabButton *tab = new TabButton( this, text, 0, 0 );
	tabArea->insertChildAt( tab, 0 );
	tab->setButtonGroup( buttonGroup );

	Panel *page = new Panel( 0, 0, 20, 20 );
	clientArea->insertChildAt( page, 0 );
	page->setVisible( false );

	if( selectedTab == nullptr )
	{
		page->setVisible( true );
		tab->setSelected( true );
		selectedTab = tab;
		selectedPanel = page;
	}

	recomputeLayout();
	return page;
}

void TabPanel::setSelectedTab( Panel *tab )
{
	if( tab == nullptr || tab == selectedTab )
		return;

	int count = tabArea->getChildCount();
	for( int i = 0; i < count; i++ )
	{
		if( tabArea->getChild( i ) != tab )
			continue;

		if( selectedPanel )
			selectedPanel->setVisible( false );
		selectedPanel = clientArea->getChild( i );
		selectedPanel->setVisible( true );
		selectedTab = tab;
		break;
	}

	recomputeLayout();
}

void TabPanel::setSize( int wide, int tall )
{
	Panel::setSize( wide, tall );
	recomputeLayout();
}

void TabPanel::recomputeLayoutTop()
{
	int paintWide, paintTall;

	getPaintSize( paintWide, paintTall );

	int count = tabArea->getChildCount();
	int minx = 5;
	int maxx = paintWide - minx;
	int x = 5, y = 0;
	int rowFirst = count - 1; // leftmost tab index of the current row

	// walk tabs from the last index down to 0 (addTab call order, left to right)
	for( int i = count - 1; i >= 0; i-- )
	{
		Panel *tab = tabArea->getChild( i );

		int wide, tall;
		tab->getPreferredSize( wide, tall );
		tab->setSize( wide, tall );

		if( i != rowFirst && x + wide > maxx )
		{
			// justify the finished row (rowFirst .. i+1): spread the unused
			// width across its tabs, widening each and shifting it right by the
			// cumulative widening of the tabs before it
			int n = rowFirst - i;
			int leftover = maxx - x;
			int extra = leftover / n;
			int error = leftover - n * extra;
			int shift = 0;

			for( int j = rowFirst; j > i; j-- )
			{
				Panel *rowTab = tabArea->getChild( j );
				int px, py, tw, th;

				rowTab->getPos( px, py );
				rowTab->getSize( tw, th );
				int add = extra + ( j == rowFirst ? error : 0 );
				rowTab->setPos( px + shift, py );
				rowTab->setSize( tw + add, th );
				shift += add;
			}

			// start a new row above
			// each upper row is 5px narrower and overlaps the row below by 4 pixels
			maxx -= minx;
			x = 5;
			y -= tall - 4;
			rowFirst = i;
		}

		tab->setPos( x, y );
		x += wide - 1; // adjacent tabs overlap by 1 pixel
	}

	// normalize so the top row sits at y = 0
	int miny = 0;
	for( int i = 0; i < count; i++ )
	{
		int px, py;

		tabArea->getChild( i )->getPos( px, py );
		if( py < miny )
			miny = py;
	}
	for( int i = 0; i < count; i++ )
	{
		Panel *tab = tabArea->getChild( i );
		int px, py;

		tab->getPos( px, py );
		tab->setPos( px, py - miny );
	}

	int tabWide, tabTall;
	tabArea->getChild( 0 )->getSize( tabWide, tabTall );
	int tabAreaHeight = tabTall - miny;

	tabArea->setBounds( 0, 5, paintWide, tabAreaHeight );
	clientArea->setBounds( 0, tabAreaHeight + 4, paintWide, paintTall - tabAreaHeight - 5 );

	int clientWide, clientTall;
	clientArea->getSize( clientWide, clientTall );
	for( int i = 0; i < clientArea->getChildCount(); i++ )
	{
		Panel *page = clientArea->getChild( i );

		page->setBounds( 5, 5, clientWide - 10, clientTall - 10 );
		page->invalidateLayout( false );
	}
}

void TabPanel::recomputeLayout()
{
	if( tabArea->getChildCount() != 0 )
	{
		if( tabPlacement == TOP )
			recomputeLayoutTop();

		// NOTE: no other tabPlacement is supported
		repaint();
	}
}
