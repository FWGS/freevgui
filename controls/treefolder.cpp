// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/treefolder.h"
#include "label.h"
#include "signals.h"
#include "layout.h"

using namespace vgui;

namespace // tree-folder label toggle handler and vertical layout
{
class TreeFolderLabelHandler : public InputSignalAdapter
{
	TreeFolder *tf;
public:
	TreeFolderLabelHandler( TreeFolder *tf ) : tf( tf ) {}

	virtual void mousePressed( MouseCode, Panel * ) override
	{
		tf->setOpened( !tf->isOpened( ));
	}
};

class TreeFolderVerticalLayout : public Layout
{
	int hgap, vgap;

public:
	TreeFolderVerticalLayout( int hgap, int vgap ) : hgap( hgap ), vgap( vgap ) {}

	virtual void performLayout( Panel *panel ) override
	{
		int count = panel->getChildCount();
		if( count == 0 )
			return;

		int y = 0, maxx = 0, labelWide = 0, labelTall = 0;

		for( int i = 0; i < count; i++ )
		{
			Panel *child = panel->getChild( i );
			int x = i == 0 ? 0 : hgap;

			// nested folders re-lay-out before we measure them
			TreeFolder *childFolder = dynamic_cast<TreeFolder *>( child );
			if( childFolder )
				childFolder->invalidateLayout( true );

			int childWide, childTall;
			child->getSize( childWide, childTall );
			child->setPos( x, y );

			y += childTall + vgap;
			if( x + childWide > maxx )
				maxx = x + childWide;

			if( i == 0 )
			{
				labelWide = childWide;
				labelTall = childTall;
			}
		}

		TreeFolder *folder = dynamic_cast<TreeFolder *>( panel );
		if( folder )
		{
			if( folder->isOpened( ))
				panel->setSize( maxx + 2, y );
			else
				panel->setSize( labelWide, labelTall );
		}
	}
};
}

TreeFolder::TreeFolder( const char *name ) : Panel( 0, 0, 500, 500 )
{
	init( name );
}

TreeFolder::TreeFolder( const char *name, int x, int y ) : Panel( x, y, 500, 500 )
{
	init( name );
}

void TreeFolder::init( const char *name )
{
	opened = false;

	Label *label = new Label( name, 0, 0 );
	label->addInputSignal( new TreeFolderLabelHandler( this ));
	label->setParent( this );

	setLayout( new TreeFolderVerticalLayout( 30, 3 ));
}

void TreeFolder::setOpened( bool open )
{
	if( opened == open )
		return;

	opened = open;

	// invalidate layout up the folder chain, tracking the topmost folder
	TreeFolder *top = nullptr;

	for( Panel *p = this; p; p = p->getParent( ))
	{
		TreeFolder *folder = dynamic_cast<TreeFolder *>( p );

		if( folder )
		{
			folder->invalidateLayout( true );
			top = folder;
		}
	}

	if( top )
		top->repaintParent();
}

void TreeFolder::setOpenedTraverse( bool open )
{
	// quirk: the local node is always opened
	setOpened( true );

	for( int i = 0; i < getChildCount(); i++ )
	{
		TreeFolder *folder = dynamic_cast<TreeFolder *>( getChild( i ));

		if( folder )
			folder->setOpenedTraverse( open );
	}
}

bool TreeFolder::isOpened()
{
	return opened;
}

void TreeFolder::paintBackground()
{
	int count = getChildCount();
	if( count <= 1 )
		return;

	int x0 = 15, x1 = 30, y0 = 10;
	int y1 = y0;

	drawSetColor( Scheme::SC_BLACK );

	// horizontal stubs from the spine out to each child
	for( int i = 1; i < count; i++ )
	{
		Panel *child = getChild( i );
		int cx, cy, cw, ch;
		child->getBounds( cx, cy, cw, ch );

		TreeFolder *folder = dynamic_cast<TreeFolder *>( child );
		int y = folder ? cy + 10 : cy + ch / 2;

		drawFilledRect( x0, y, x1, y + 1 );
		y1 = y;
	}

	// vertical spine down to the last stub
	drawFilledRect( x0, y0, x0 + 1, y1 );

	// expand/collapse boxes over the spine for each sub-folder
	for( int i = 1; i < count; i++ )
	{
		Panel *child = getChild( i );

		TreeFolder *folder = dynamic_cast<TreeFolder *>( child );
		if( !folder )
			continue;

		int cx, cy, cw, ch;
		child->getBounds( cx, cy, cw, ch );
		int y = cy + 10;

		drawSetColor( Scheme::SC_WHITE );
		drawFilledRect( x0 - 5, y - 5, x0 + 6, y + 6 );
		drawSetColor( Scheme::SC_BLACK );
		drawOutlinedRect( x0 - 5, y - 5, x0 + 6, y + 6 );
		drawFilledRect( x0 - 3, y, x0 + 4, y + 1 );

		if( !folder->isOpened( ))
			drawFilledRect( x0, y - 3, x0 + 1, y + 4 );
	}
}
