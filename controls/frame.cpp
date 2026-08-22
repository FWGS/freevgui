// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <string.h>

#include "controls/frame.h"
#include "app.h"
#include "surface.h"

using namespace vgui;

namespace // private drag/resize signals and button handlers, not part of the ABI
{
class DraggerSignal : public InputSignalAdapter
{
public:
	DraggerSignal( Panel *panel, int mx, int my, int mw, int mh ) :
		panel( panel ), mx( mx ), my( my ), mw( mw ), mh( mh ), dragging( false )
	{
	}

	virtual void mousePressed( MouseCode, Panel * ) override
	{
		Panel *target = resolveTarget();
		if( !target )
			return;

		dragging = true;

		App::getInstance()->getCursorPos( originX, originY );
		target->getPos( startX, startY );
		target->getSize( startW, startH );

		// when we have a parent, reparent a child bringing it to the front
		Panel *parent = target->getParent();
		if( parent )
		{
			int ax0, ay0, ax1, ay1;
			parent->getAbsExtents( ax0, ay0, ax1, ay1 );
			App::getInstance()->setMouseArena( ax0, ay0, ax1, ay1, true );

			parent->removeChild( target );
			parent->addChild( target );
		}

		// capture, focus and repaint always run
		App::getInstance()->setMouseCapture( panel );
		panel->requestFocus();
		App::getInstance()->repaintAll();
	}

	virtual void cursorMoved( int, int, Panel * ) override
	{
		if( !dragging )
			return;

		Frame *frame = getFrame();
		if( !frame )
			return;

		// resize grips (any edge multiplier) gate on isSizeable(); the caption move grip
		// (position-only) gates on isMoveable(), independently. the client (no config) is raise-only.
		bool resize = mw || mh;
		bool move = !resize && ( mx || my );

		if( resize && !frame->isSizeable())
			return;
		if( move && !frame->isMoveable())
			return;
		if( !resize && !move )
			return;

		Panel *target = frame->isInternal() ? (Panel *)frame : frame->getParent();
		if( !target )
			return;

		int curX, curY;
		App::getInstance()->getCursorPos( curX, curY );

		int dx = curX - originX;
		int dy = curY - originY;

		int nx = startX + dx * mx;
		int ny = startY + dy * my;
		int nw = startW + dx * mw;
		int nh = startH + dy * mh;

		// Panel::setSize already clamps up to the target's setMinimumSize, so no manual clamp here
		target->setBounds( nx, ny, nw, nh );
		App::getInstance()->repaintAll();
	}

	virtual void mouseReleased( MouseCode, Panel * ) override
	{
		dragging = false;
		App::getInstance()->setMouseArena( 0, 0, 0, 0, false );
		App::getInstance()->setMouseCapture( nullptr );
	}

private:
	Frame *getFrame()
	{
		// RTTI is a requirement, sorry.
		return dynamic_cast<Frame *>( panel->getParent());
	}

	Panel *resolveTarget()
	{
		Frame *frame = getFrame();
		if( !frame )
			return nullptr;

		return frame->isInternal() ? (Panel *)frame : frame->getParent();
	}

	Panel *panel;
	int mx, my, mw, mh;
	bool dragging;
	int originX, originY;
	int startX, startY, startW, startH;
};

class CloseButtonHandler : public ActionSignal
{
public:
	CloseButtonHandler( Frame *frame ) : frame( frame ) {}

	virtual void actionPerformed( Panel * ) override
	{
		frame->fireClosingSignal();
	}

private:
	Frame *frame;
};

class MinimizeButtonHandler : public ActionSignal
{
public:
	MinimizeButtonHandler( Frame *frame ) : frame( frame ) {}

	virtual void actionPerformed( Panel * ) override
	{
		frame->fireMinimizingSignal();
	}

private:
	Frame *frame;
};

static Panel *makeGrip( Frame *frame, int x, int y, int w, int h, Cursor::DefaultCursor dc, int mx, int my, int mw, int mh )
{
	Panel *grip = new Panel( x, y, w, h );

	grip->setCursor( new Cursor( dc ));
	grip->addInputSignal( new DraggerSignal( grip, mx, my, mw, mh ));
	grip->setPaintBorderEnabled( false );
	grip->setPaintBackgroundEnabled( false );
	grip->setPaintEnabled( false );
	grip->setParent( frame );

	return grip;
}
}

Frame::Frame( int x, int y, int wide, int tall ) :
	Panel( x, y, wide, tall ), title( nullptr ), internal( true ), moveable( true ), sizeable( true )
{
	setTitle( "Untitled" );
	setMinimumSize( 64, 33 );

	// grips + caption (z-order = creation order)
	// per-grip (mx,my,mw,mh) selects edges
	topGrip         = makeGrip( this, 15,        0,         wide - 30, 5,         Cursor::DC_SIZENS,   0, 1,  0, -1 );
	bottomGrip      = makeGrip( this, 15,        tall - 5,  wide - 30, 5,         Cursor::DC_SIZENS,   0, 0,  0,  1 );
	leftGrip        = makeGrip( this, 0,         15,        5,         tall - 30, Cursor::DC_SIZEWE,   1, 0, -1,  0 );
	rightGrip       = makeGrip( this, wide - 5,  15,        5,         tall - 30, Cursor::DC_SIZEWE,   0, 0,  1,  0 );
	topLeftGrip     = makeGrip( this, 0,         0,         15,        15,        Cursor::DC_SIZENWSE, 1, 1, -1, -1 );
	topRightGrip    = makeGrip( this, wide - 15, 0,         15,        15,        Cursor::DC_SIZENESW, 0, 1,  1, -1 );
	bottomLeftGrip  = makeGrip( this, 0,         tall - 15, 15,        15,        Cursor::DC_SIZENESW, 1, 0, -1,  1 );
	bottomRightGrip = makeGrip( this, wide - 15, tall - 15, 15,        15,        Cursor::DC_SIZENWSE, 0, 0,  1,  1 );
	captionGrip     = makeGrip( this, 5,         5,         wide - 10, 23,        Cursor::DC_SIZEALL,  1, 1,  0,  0 );

	client = new Panel( 5, 29, wide - 10, tall - 34 );
	client->addInputSignal( new DraggerSignal( client, 0, 0, 0, 0 ));
	client->setParent( this );

	// these use Marlett font, that's why they make no sense
	trayButton = new Button( ".", wide - 85, 8, 18, 18 );
	trayButton->setParent( this );

	minimizeButton = new Button( "2", wide - 65, 8, 18, 18 );
	minimizeButton->setFont( Scheme::SF_SECONDARY );
	minimizeButton->addActionSignal( new MinimizeButtonHandler( this ));
	minimizeButton->setParent( this );

	maximizeButton = new Button( "1", wide - 45, 8, 18, 18 );
	maximizeButton->setFont( Scheme::SF_SECONDARY );
	maximizeButton->setParent( this );

	closeButton = new Button( "r", wide - 25, 8, 18, 18 );
	closeButton->setFont( Scheme::SF_SECONDARY );
	closeButton->addActionSignal( new CloseButtonHandler( this ));
	closeButton->setParent( this );

	menuButton = new Button( "s", 7, 8, 18, 18 );
	menuButton->setFont( Scheme::SF_SECONDARY );
	menuButton->setParent( this );
}

void Frame::setSize( int wide, int tall )
{
	if( getWide() == wide && getTall() == tall )
		return;

	Panel::setSize( wide, tall );

	int w = size[0], h = size[1];

	// FIXME: I hate this kind of code, could be made common with makeGrip above
	topGrip->setBounds(         15,     0,      w - 30, 5 );
	bottomGrip->setBounds(      15,     h - 5,  w - 30, 5 );
	leftGrip->setBounds(        0,      15,     5,      h - 30 );
	rightGrip->setBounds(       w - 5,  15,     5,      h - 30 );
	// topLeftGrip->setBounds(     0,      0,      15,     15 ); // could be skipped?
	topRightGrip->setBounds(    w - 15, 0,      15,     15 );
	bottomLeftGrip->setBounds(  0,      h - 15, 15,     15 );
	bottomRightGrip->setBounds( w - 15, h - 15, 15,     15 );
	captionGrip->setBounds(     5,      5,      w - 10, 23 );
	client->setBounds(          5,      29,     w - 10, h - 34 );

	trayButton->setBounds( w - 85, 8, 18, 18 );
	minimizeButton->setBounds( w - 65, 8, 18, 18 );
	maximizeButton->setBounds( w - 45, 8, 18, 18 );
	closeButton->setBounds( w - 25, 8, 18, 18 );
	// menu button position is const

	client->invalidateLayout( false );
}

void Frame::setInternal( bool state )
{
	internal = state;
}

bool Frame::isInternal()
{
	return internal;
}

Panel *Frame::getClient()
{
	return client;
}

void Frame::setTitle( const char *newTitle )
{
	delete[] title;
	title = vgui_strdup( newTitle );
	repaint();
}

void Frame::getTitle( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, title );
}

void Frame::setMoveable( bool state )
{
	moveable = state;
}

void Frame::setSizeable( bool state )
{
	sizeable = state;
}

bool Frame::isMoveable()
{
	return moveable;
}

bool Frame::isSizeable()
{
	return sizeable;
}

void Frame::addFrameSignal( FrameSignal *s )
{
	frameSignals.putElement( s );
}

void Frame::setVisible( bool state )
{
	Panel::setVisible( state );

	if( getParent())
		getParent()->repaint();
}

void Frame::setMenuButtonVisible( bool state )
{
	menuButton->setVisible( state );
}

void Frame::setTrayButtonVisible( bool state )
{
	trayButton->setVisible( state );
}

void Frame::setMinimizeButtonVisible( bool state )
{
	minimizeButton->setVisible( state );
}

void Frame::setMaximizeButtonVisible( bool state )
{
	maximizeButton->setVisible( state );
}

void Frame::setCloseButtonVisible( bool state )
{
	closeButton->setVisible( state );
}

void Frame::paintBackground()
{
	int wide = size[0], tall = size[1];

	// focused = surface has focus && a panel is focused && this frame is ancestor-or-self of it
	bool focused = false;
	SurfaceBase *sb = getSurfaceBase();
	if( sb && sb->hasFocus())
	{
		for( Panel *f = App::getInstance()->getFocus(); f; f = f->getParent())
		{
			if( f == this )
			{
				focused = true;
				break;
			}
		}
	}

	Scheme::SchemeColor captionFill = focused ? Scheme::SC_PRIMARY3 : Scheme::SC_SECONDARY3;
	Scheme::SchemeColor accent      = focused ? Scheme::SC_PRIMARY1 : Scheme::SC_SECONDARY1;
	Scheme::SchemeColor bevel       = focused ? Scheme::SC_PRIMARY2 : Scheme::SC_SECONDARY2;

	// 1. caption fill
	drawSetColor( captionFill );
	drawFilledRect( 5, 5, wide - 5, 28 );

	// 2. grab lines, first row (white)
	drawSetColor( Scheme::SC_WHITE );
	drawFilledRect( 120, 10, wide - 70, 11 );
	drawFilledRect( 120, 15, wide - 70, 16 );
	drawFilledRect( 120, 20, wide - 70, 21 );

	// 3. grab lines, second row (accent, +2px)
	drawSetColor( accent );
	drawFilledRect( 120, 12, wide - 70, 13 );
	drawFilledRect( 120, 17, wide - 70, 18 );
	drawFilledRect( 120, 22, wide - 70, 23 );

	// 4. outer border (accent) - left/right edges stop 1px short, two 1px corner notches
	drawFilledRect( 1,        0,        wide - 1, 5 );
	drawFilledRect( 1,        tall - 5, wide - 1, tall );
	drawFilledRect( 0,        1,        5,        tall - 1 );
	drawFilledRect( wide - 5, 1,        wide,     tall - 1 );
	drawFilledRect( 5,        5,        6,        6 );
	drawFilledRect( wide - 6, 5,        wide - 5, 6 );
	drawFilledRect( 5,        28,       wide - 5, 29 );

	// 5. inner black lines (~13px inset)
	drawSetColor( Scheme::SC_BLACK );
	drawFilledRect( 13,       2,        wide - 13, 3 );
	drawFilledRect( 13,       tall - 3, wide - 13, tall - 2 );
	drawFilledRect( 2,        13,       3,         tall - 13 );
	drawFilledRect( wide - 3, 13,       wide - 2,  tall - 13 );

	// 6. bevel highlight (~14px inset)
	drawSetColor( bevel );
	drawFilledRect( 14,       3,        wide - 12, 4 );
	drawFilledRect( 14,       tall - 2, wide - 12, tall - 1 );
	drawFilledRect( 3,        14,       4,         tall - 12 );
	drawFilledRect( wide - 2, 14,       wide - 1,  tall - 12 );

	// 7. title
	if( title )
	{
		drawSetTextFont( Scheme::SF_PRIMARY1 );
		drawSetTextColor( Scheme::SC_BLACK );
		drawPrintText( 28, 7, title, strlen( title ));
	}
}

void Frame::fireClosingSignal()
{
	for( FrameSignal *s : frameSignals )
		s->closing( this );
}

void Frame::fireMinimizingSignal()
{
	for( FrameSignal *s : frameSignals )
		s->minimizing( this, false );
}
