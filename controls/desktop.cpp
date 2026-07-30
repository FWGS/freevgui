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
	setBgColor( 0, 128, 128, 0 );
	setPaintBorderEnabled( false );
	setPaintBackgroundEnabled( false );
	setPaintEnabled( false );

	_background = new Panel( 0, 0, wide, tall - 36 );
	_background->setBgColor( 0, 128, 128, 0 );
	addChild( _background );

	_foreground = new Panel( 0, 0, wide, tall - 36 );
	_foreground->setPaintBorderEnabled( false );
	_foreground->setPaintBackgroundEnabled( false );
	_foreground->setPaintEnabled( false );
	addChild( _foreground );

	_taskBar = new TaskBar( 0, tall - 36, wide, 36 );
	addChild( _taskBar );

	_cascade[0] = 50;
	_cascade[1] = 50;
}

void Desktop::setSize( int wide, int tall )
{
	Panel::setSize( wide, tall );

	getPaintSize( wide, tall );
	_background->setBounds( 0, 0, wide, tall - 36 );
	_foreground->setBounds( 0, 0, wide, tall - 36 );
	_taskBar->setBounds( 0, tall - 36, wide, 36 );
}

void Desktop::iconActivated( DesktopIcon *icon )
{
	Frame *frame = icon->getMiniApp()->createInstance();

	if( !frame )
		return;

	frame->setPos( _cascade[0], _cascade[1] );
	_foreground->addChild( frame );
	_taskBar->addFrame( frame );
	frame->requestFocus();

	_cascade[0] += 25;
	_cascade[1] += 50;

	if( _cascade[1] > 400 )
	{
		_cascade[0] = 50;
		_cascade[1] = 50;
	}
}

void Desktop::addIcon( DesktopIcon *icon )
{
	icon->setDesktop( this );
	_foreground->addChild( icon );
	icon->setPos( 10, 10 );
	_desktopIconDar.addElement( icon );
}

void Desktop::arrangeIcons()
{
	int x = 15, y = 10;

	for( DesktopIcon *icon : _desktopIconDar )
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
	return _background;
}

Panel *Desktop::getForeground()
{
	return _foreground;
}
