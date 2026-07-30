// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/treefolder.h"
#include "label.h"
#include "signals.h"
#include "layout.h"

using namespace vgui;

TreeFolder::TreeFolder( const char *name ) : Panel()
{
}

TreeFolder::TreeFolder( const char *name, int x, int y ) : Panel( x, y, 0, 0 )
{
}

void TreeFolder::init( const char *name )
{
}

void TreeFolder::paintBackground()
{
}

void TreeFolder::setOpenedTraverse( bool open )
{
}

void TreeFolder::setOpened( bool open )
{
}

bool TreeFolder::isOpened()
{
	return false;
}

#if 0

class FooTreeFolderDefaultHandler : public InputSignal
{
protected:
	TreeFolder *_treeFolder;
public:
	FooTreeFolderDefaultHandler( TreeFolder *tf ) :
		_treeFolder( tf )
	{
	}

	void cursorMoved( int x, int y, Panel *p ) override;
	void cursorEntered( Panel *p ) override;
	void cursorExited( Panel *p ) override;
	void mousePressed( MouseCode code, Panel *p ) override;
	void mouseDoublePressed( MouseCode code, Panel *p ) override;
	void mouseReleased( MouseCode code, Panel *p ) override;
	void mouseWheeled( int y, Panel *p ) override;
	void keyPressed( KeyCode code, Panel *p ) override;
	void keyTyped( KeyCode code, Panel *p ) override;
	void keyReleased( KeyCode code, Panel *p ) override;
	void keyFocusTicked( Panel *p ) override;
};

class FooTabFolderVerticalLayout : public Layout
{
protected:
	int _hgap;
	int _vgap;
public:
	FooTabFolderVerticalLayout( int hgap, int vgap ) :
		_hgap( hgap ), _vgap( vgap )
	{

	}

	void performLayout( Panel *p ) override;
};

TreeFolder::TreeFolder(const char *name) :
	TreeFolder( name, 0, 0 )
{
}

TreeFolder::TreeFolder(const char *name, int x, int y) :
	Panel( x, y, 500, 500 )
{
	init( name );
}

void TreeFolder::init( const char *name )
{
	_opened = false;

	Label *l = new Label( name, 0, 0 );
	l->addInputSignal( new FooTreeFolderDefaultHandler( this ));
	l->setParent( this );

	setLayout( new FooTabFolderVerticalLayout( 0x1e, 3 ));
}

void TreeFolder::setOpenedTraverse(bool open)
{
	setOpened( true );

	for( int i = 0; i < getChildCount(); i++ )
	{
		TreeFolder *tf = dynamic_cast<TreeFolder *>( getChild( i ));
		if( tf )
			tf->setOpenedTraverse( open );
	}
}

void TreeFolder::setOpened(bool open)
{
	if( _opened == open )
		return;

	_opened = open;

	TreeFolder *top = nullptr;

	for( Panel *p = this; p; p = p->getParent( ))
	{
		TreeFolder *tf = dynamic_cast<TreeFolder*>( p );

		if( tf )
		{
			tf->invalidateLayout( true );
			top = tf;
		}
	}

	if( top )
		top->repaintParent();
}

bool TreeFolder::isOpened()
{
	return _opened;
}
#endif
