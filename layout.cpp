// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "layout.h"
#include "panel.h"

using namespace vgui;

// this class is private to VGUI
class BorderLayoutInfo : public LayoutInfo
{
public:
	virtual LayoutInfo *getThis() override
	{
		return this;
	}

	BorderLayout::Alignment _alignment;
};

LayoutInfo *BorderLayout::createLayoutInfo( Alignment alignment )
{
	BorderLayoutInfo *bli = new BorderLayoutInfo();
	bli->_alignment = alignment;

	return bli;
}

BorderLayout::BorderLayout( int inset ) :
	_inset( inset )
{

}

void BorderLayout::performLayout( Panel *p )
{
	int w, h;
	int max[5] = {};

	p->getSize( w, h );

	for( int i = 0; i < p->getChildCount(); i++ )
	{
		BorderLayoutInfo *bli = dynamic_cast<BorderLayoutInfo *>( p->getChild( i )->getLayoutInfo( ));

		if( !bli )
			continue;

		Panel *child = p->getChild( i );

		switch( bli->_alignment )
		{
		case TOP:
		case BOTTOM:
		case RIGHT: // a1ba: probably a mistake?
			if( child->getTall() > max[bli->_alignment] )
				max[bli->_alignment] = child->getTall();
			break;
		case LEFT:
			if( child->getWide() > max[bli->_alignment] )
				max[bli->_alignment] = child->getWide();
			break;
		}
	}

	int x0 = max[LEFT] + _inset;
	int y0 = max[TOP] + _inset;
	int x1 = w - max[RIGHT] - _inset;
	int y1 = h - max[BOTTOM] - _inset;

	for( int i = 0; i < p->getChildCount(); i++ )
	{
		BorderLayoutInfo *bli = dynamic_cast<BorderLayoutInfo *>( p->getChild( i )->getLayoutInfo( ));

		if( !bli )
			continue;

		Panel *child = p->getChild( i );

		switch( bli->_alignment )
		{
		case CENTER:
			child->setBounds( x0, y0, x1 - x0, y1 - y0 );
			break;
		case TOP:
			child->setBounds( 0, 0, w, y0 );
			break;
		case BOTTOM:
			child->setBounds( 0, y1, w, h - y1 );
			break;
		case RIGHT:
			child->setBounds( x1, y0, w - x1, y1 - y0 );
			break;
		case LEFT:
			child->setBounds( 0, y0, x0, y1 - y0 );
			break;
		}
	}
}

FlowLayout::FlowLayout( int hgap ) : _hgap( hgap )
{

}

void FlowLayout::performLayout( Panel *p )
{
	int newx = 0;
	for( int i = 0; i < p->getChildCount(); i++ )
	{
		Panel *child = p->getChild( i );
		int x, y, w, h;

		child->getBounds( x, y, w, h );
		child->setPos( newx, y );

		newx += _hgap + w;
	}
}

StackLayout::StackLayout(int vgap, bool fitWide) :
	_vgap( vgap ), _fitWide( fitWide )
{

}

void StackLayout::performLayout( Panel *p )
{
	int newy = 0;

	for( int i = 0; i < p->getChildCount(); i++ )
	{
		Panel *child = p->getChild( i );
		int x, y, w, h;

		child->getBounds( x, y, w, h );
		child->setPos( x, newy );

		if( _fitWide )
		{
			int pwide, ptall;

			p->getPaintSize( pwide, ptall );
			child->setSize( pwide, ptall );
		}

		newy += _vgap + h;
	}
}

void Layout::performLayout(Panel *)
{
	return;
}
