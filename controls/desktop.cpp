// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include <time.h>
#include <string.h>

#include "controls/desktop.h"
#include "app.h"

using namespace vgui;

MiniApp::MiniApp()
{
	_name = (char *)"";
}

void MiniApp::getName( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, _name );
}

void MiniApp::setName( const char *name )
{
	_name = vgui_strdup( name );
}

namespace
{
class TrayClock : public Panel, public TickSignal
{
public:
	TrayClock( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
	{
		lastSecond = -1;
		text[0] = '\0';
		App::getInstance()->addTickSignal( this );
	}

	virtual void ticked() override
	{
		time_t now = time( NULL );
		struct tm *lt = localtime( &now );

		if( lt->tm_sec == lastSecond )
			return;

		lastSecond = lt->tm_sec;
		strftime( text, sizeof( text ), "%I:%M:%S %p", lt );
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
		App::getInstance()->requestFocus( NULL );
	}

	virtual void focusChanged( bool lost, Panel * ) override
	{
		button->setSelected( !lost );
	}

private:
	Button *button;
	Frame  *frame;
};

}

TaskBar::TaskBar( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
	setBorder( new RaisedBorder());

	_tray = new Panel( 100, 0, 120, 26 );
	_tray->setBorder( new LoweredBorder());
	addChild( _tray );

	_tray->addChild( new TrayClock( 25, 2, 85, 20 ));
}

void TaskBar::addFrame( Frame *frame )
{
	_frameDar.addElement( frame );

	char title[256];
	frame->getTitle( title, sizeof( title ));

	Button *button = new Button( title, 2, 2 );
	TaskButtonHandler *handler = new TaskButtonHandler( button, frame );

	addChild( button );
	button->addActionSignal( handler );
	frame->addFrameSignal( handler );
	frame->addFocusChangeSignal( handler );

	_taskButtonDar.addElement( button );
	invalidateLayout( false );
}

void TaskBar::performLayout()
{
	int wide, tall;
	getPaintSize( wide, tall );

	int trayWide, trayTall;
	_tray->getSize( trayWide, trayTall );
	_tray->setBounds( wide - trayWide - 3, 2, trayWide, tall - 4 );

	int x = 3;
	for( Button *button : _taskButtonDar )
	{
		int bx, by;
		button->getBounds( bx, by, wide, tall );
		button->setBounds( x, by, wide, tall );
		x += wide + 3;
	}
}

DesktopIcon::DesktopIcon( MiniApp *miniApp, Image *image ) : Panel()
{
}

void DesktopIcon::doActivate()
{
}

void DesktopIcon::setImage( Image *image )
{
}

void DesktopIcon::setDesktop( Desktop *desktop )
{
}

MiniApp *DesktopIcon::getMiniApp()
{
	return NULL;
}

void DesktopIcon::paintBackground()
{
}

Desktop::Desktop( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void Desktop::setSize( int wide, int tall )
{
}

void Desktop::iconActivated( DesktopIcon *icon )
{
}

void Desktop::addIcon( DesktopIcon *icon )
{
}

void Desktop::arrangeIcons()
{
}

Panel *Desktop::getBackground()
{
	return NULL;
}

Panel *Desktop::getForeground()
{
	return NULL;
}
