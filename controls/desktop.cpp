// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <time.h>
#include "vgui_internal.h"
#include <string.h>

#include "controls/desktop.h"
#include "app.h"
#include "font.h"

using namespace vgui;

namespace // private to VGUI classes, signal helpers and panels
{
class TrayClock : public Panel, public TickSignal
{
public:
	TrayClock( int x, int y, int wide, int tall ) :
		Panel( x, y, wide, tall ), lastSecond( -1 ), text{}
	{
		App::getInstance()->addTickSignal( this );
	}

	virtual void ticked() override
	{
		time_t now = time( nullptr );
		struct tm *lt = localtime( &now );

		if( lt->tm_sec == lastSecond )
			return;

		lastSecond = lt->tm_sec;

		// strip leading zero. Would've used strftime but standard stftime doesn't support removing leading zero.
		int hour = lt->tm_hour % 12;
		if( hour == 0 )
			hour = 12;

		snprintf( text, sizeof( text ), "%d:%02d:%02d %s", hour, lt->tm_min, lt->tm_sec, lt->tm_hour < 12 ? "AM" : "PM" );
		repaint();
	}

protected:
	virtual void paint() override
	{
		drawSetTextFont( Scheme::SF_PRIMARY1 );
		drawSetTextColor( Scheme::SC_BLACK );
		drawSetTextPos( 0, 0 );
		drawPrintText( text, strlen( text ));
	}

private:
	int  lastSecond;
	char text[32];
};

class TaskButtonHandler : public ActionSignal, public FrameSignal, public FocusChangeSignal
{
public:
	TaskButtonHandler( Button *button, Frame *frame ) :
		button( button ), frame( frame )
	{
	}

	virtual void actionPerformed( Panel * ) override
	{
		frame->setVisible( true );
		frame->requestFocus();
	}

	virtual void closing( Frame * ) override
	{
		// intentionally empty: a closed frame keeps its taskbar button
	}

	virtual void minimizing( Frame *, bool ) override
	{
		frame->setVisible( false );
		App::getInstance()->requestFocus( nullptr );
	}

	virtual void focusChanged( bool lost, Panel * ) override
	{
		button->setSelected( !lost );
	}

private:
	Button *button;
	Frame  *frame;
};

class IconDragHandler : public InputSignal
{
public:
	IconDragHandler( DesktopIcon *icon ) : icon( icon ), dragging( false )
	{
	}

	virtual void mousePressed( MouseCode, Panel * ) override
	{
		icon->getPos( origin[0], origin[1] );
		App::getInstance()->getCursorPos( start[0], start[1] );
		icon->setAsMouseCapture( true );
		icon->requestFocus();
		dragging = true;
	}

	virtual void cursorMoved( int, int, Panel * ) override
	{
		if( !dragging )
			return;

		int cur[2];
		App::getInstance()->getCursorPos( cur[0], cur[1] );
		icon->setPos( origin[0] + cur[0] - start[0], origin[1] + cur[1] - start[1] );
		icon->repaintParent();
	}

	virtual void mouseDoublePressed( MouseCode, Panel * ) override
	{
		icon->doActivate();
	}

	virtual void mouseReleased( MouseCode, Panel * ) override
	{
		dragging = false;
		icon->setAsMouseCapture( false );
	}

	virtual void cursorEntered( Panel * ) override {}
	virtual void cursorExited( Panel * ) override {}
	virtual void mouseWheeled( int, Panel * ) override {}
	virtual void keyPressed( KeyCode, Panel * ) override {}
	virtual void keyTyped( KeyCode, Panel * ) override {}
	virtual void keyReleased( KeyCode, Panel * ) override {}
	virtual void keyFocusTicked( Panel * ) override {}

private:
	DesktopIcon *icon;
	bool dragging;
	int  origin[2];
	int  start[2];
};
}

MiniApp::MiniApp() : name((char *)"" )
{
}

void MiniApp::getName( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, name );
}

void MiniApp::setName( const char *newName )
{
	name = vgui_strdup( newName );
}

TaskBar::TaskBar( int x, int y, int wide, int tall ) :
	Panel( x, y, wide, tall ), tray( new Panel( 100, 0, 120, 26 ))
{
	setBorder( new RaisedBorder());

	tray->setBorder( new LoweredBorder());
	addChild( tray );

	tray->addChild( new TrayClock( 25, 2, 85, 20 ));
}

void TaskBar::addFrame( Frame *frame )
{
	frames.addElement( frame );

	char title[256];
	frame->getTitle( title, sizeof( title ));

	Button *button = new Button( title, 2, 2 );
	TaskButtonHandler *handler = new TaskButtonHandler( button, frame );

	addChild( button );
	button->addActionSignal( handler );
	frame->addFrameSignal( handler );
	frame->addFocusChangeSignal( handler );

	taskButtons.addElement( button );
	invalidateLayout( false );
}

void TaskBar::performLayout()
{
	int wide, tall;
	getPaintSize( wide, tall );

	int trayWide, trayTall;
	tray->getSize( trayWide, trayTall );
	tray->setBounds( wide - trayWide - 3, 2, trayWide, tall - 4 );

	int x = 3;
	for( Button *button : taskButtons )
	{
		int bx, by;
		button->getBounds( bx, by, wide, tall );
		button->setBounds( x, by, wide, tall );
		x += wide + 3;
	}
}

DesktopIcon::DesktopIcon( MiniApp *miniApp, Image *image ) :
	Panel( 0, 0, 32, 50 ), desktop( nullptr ), miniApp( miniApp )
{
	setImage( image );
	addInputSignal( new IconDragHandler( this ));
}

void DesktopIcon::doActivate()
{
	if( desktop )
		desktop->iconActivated( this );
}

void DesktopIcon::setImage( Image *newImage )
{
	image = newImage;

	if( !image )
		return;

	int wide, tall;
	image->getSize( wide, tall );
	setSize( wide, tall );
}

void DesktopIcon::setDesktop( Desktop *newDesktop )
{
	desktop = newDesktop;
}

MiniApp *DesktopIcon::getMiniApp()
{
	return miniApp;
}

void DesktopIcon::paintBackground()
{
	Panel::paintBackground();

	int wide, tall;
	getPaintSize( wide, tall );

	if( image )
		image->doPaint( this );

	drawSetTextFont( Scheme::SF_PRIMARY1 );

	if( miniApp )
	{
		char name[256];
		miniApp->getName( name, sizeof( name ));

		int tWide, tTall;
		App::getInstance()->getScheme()->getFont( Scheme::SF_PRIMARY2 )->getTextSize( name, tWide, tTall );

		drawSetTextColor( 255, 255, 255, 0 );
		drawSetTextPos( wide / 2 - tWide / 2, tall - 20 );
		drawPrintText( name, strlen( name ));
	}
}

Desktop::Desktop( int x, int y, int wide, int tall ) :
	Panel( x, y, wide, tall ),
	background( new Panel( 0, 0, wide, tall - 36 )),
	foreground( new Panel( 0, 0, wide, tall - 36 )),
	taskBar( new TaskBar( 0, tall - 36, wide, 36 )),
	nextFramePos{ 50, 50 }
{
	setBgColor( 0, 128, 128, 0 );
	setPaintBorderEnabled( false );
	setPaintBackgroundEnabled( false );
	setPaintEnabled( false );

	background->setBgColor( 0, 128, 128, 0 );
	addChild( background );

	foreground->setPaintBorderEnabled( false );
	foreground->setPaintBackgroundEnabled( false );
	foreground->setPaintEnabled( false );
	addChild( foreground );

	addChild( taskBar );
}

void Desktop::setSize( int wide, int tall )
{
	Panel::setSize( wide, tall );

	getPaintSize( wide, tall );
	background->setBounds( 0, 0, wide, tall - 36 );
	foreground->setBounds( 0, 0, wide, tall - 36 );
	taskBar->setBounds( 0, tall - 36, wide, 36 );
}

void Desktop::iconActivated( DesktopIcon *icon )
{
	Frame *frame = icon->getMiniApp()->createInstance();

	if( !frame )
		return;

	frame->setPos( nextFramePos[0], nextFramePos[1] );
	foreground->addChild( frame );
	taskBar->addFrame( frame );
	frame->requestFocus();

	nextFramePos[0] += 25;
	nextFramePos[1] += 50;

	if( nextFramePos[1] > 400 )
	{
		nextFramePos[0] = 50;
		nextFramePos[1] = 50;
	}
}

void Desktop::addIcon( DesktopIcon *icon )
{
	icon->setDesktop( this );
	foreground->addChild( icon );
	icon->setPos( 10, 10 );
	desktopIcons.addElement( icon );
}

void Desktop::arrangeIcons()
{
	int x = 15, y = 10;

	for( DesktopIcon *icon : desktopIcons )
	{
		icon->setPos( x, y );
		y += 60;

		if( y > 340 )
		{
			x += 50;
			y = 10;
		}
	}
}

Panel *Desktop::getBackground()
{
	return background;
}

Panel *Desktop::getForeground()
{
	return foreground;
}
