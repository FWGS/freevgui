// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "app.h"
#include "vgui_internal.h"
#include "surface.h"
#include "signals.h"
#include "panel.h"
#include "font.h"

using namespace vgui;

App* App::singleton = nullptr;
static const char* staticKeyTrans[KEY_LAST] =
{
	"0)KEY_0",
	"1!KEY_1",
	"2@KEY_2",
	"3#KEY_3",
	"4$KEY_4",
	"5%KEY_5",
	"6^KEY_6",
	"7&KEY_7",
	"8*KEY_8",
	"9(KEY_9",
	"aAKEY_A",
	"bBKEY_B",
	"cCKEY_C",
	"dDKEY_D",
	"eEKEY_E",
	"fFKEY_F",
	"gGKEY_G",
	"hHKEY_H",
	"iIKEY_I",
	"jJKEY_J",
	"kKKEY_K",
	"lLKEY_L",
	"mMKEY_M",
	"nNKEY_N",
	"oOKEY_O",
	"pPKEY_P",
	"qQKEY_Q",
	"rRKEY_R",
	"sSKEY_S",
	"tTKEY_T",
	"uUKEY_U",
	"vVKEY_V",
	"wWKEY_W",
	"xXKEY_X",
	"yYKEY_Y",
	"zZKEY_Z",
	"0\0KEY_PAD_0",
	"1\0KEY_PAD_1",
	"2\0KEY_PAD_2",
	"3\0KEY_PAD_3",
	"4\0KEY_PAD_4",
	"5\0KEY_PAD_5",
	"6\0KEY_PAD_6",
	"7\0KEY_PAD_7",
	"8\0KEY_PAD_8",
	"9\0KEY_PAD_9",
	"//KEY_PAD_DIVIDE",
	"**KEY_PAD_MULTIPLY",
	"--KEY_PAD_MINUS",
	"++KEY_PAD_PLUS",
	"\0\0KEY_PAD_ENTER",
	".\0KEY_PAD_DECIMAL",
	"[{KEY_LBRACKET",
	"]}KEY_RBRACKET",
	",:KEY_SEMICOLON",
	"'\"KEY_APOSTROPHE",
	"`~KEY_BACKQUOTE",
	",<KEY_COMMA",
	".>KEY_PERIOD",
	"/?KEY_SLASH",
	"\\|KEY_BACKSLASH",
	"-_KEY_MINUS",
	"=+KEY_EQUAL",
	"\0\0KEY_ENTER",
	"  KEY_SPACE",
	"\0\0KEY_BACKSPACE",
	"\0\0KEY_TAB",
	"\0\0KEY_CAPSLOCK",
	"\0\0KEY_NUMLOCK",
	"\0\0KEY_ESCAPE",
	"\0\0KEY_SCROLLLOCK",
	"\0\0KEY_INSERT",
	"\0\0KEY_DELETE",
	"\0\0KEY_HOME",
	"\0\0KEY_END",
	"\0\0KEY_PAGEUP",
	"\0\0KEY_PAGEDOWN",
	"\0\0KEY_BREAK",
	"\0\0KEY_LSHIFT",
	"\0\0KEY_RSHIFT",
	"\0\0KEY_LALT",
	"\0\0KEY_RALT",
	"\0\0KEY_LCONTROL",
	"\0\0KEY_RCONTROL",
	"\0\0KEY_LWIN",
	"\0\0KEY_RWIN",
	"\0\0KEY_APP",
	"\0\0KEY_UP",
	"\0\0KEY_LEFT",
	"\0\0KEY_DOWN",
	"\0\0KEY_RIGHT",
	"\0\0KEY_F1",
	"\0\0KEY_F2",
	"\0\0KEY_F3",
	"\0\0KEY_F4",
	"\0\0KEY_F5",
	"\0\0KEY_F6",
	"\0\0KEY_F7",
	"\0\0KEY_F8",
	"\0\0KEY_F9",
	"\0\0KEY_F10",
	"\0\0KEY_F11",
	"\0\0KEY_F12",
};

App::App( bool externalMain )
{
	init();
	externalMainLoop = externalMain;
}

App::App() : App( false ) { }

App *App::getInstance()
{
	return singleton;
}

void App::start()
{
	if( !externalMainLoop )
	{
		run();
		for( int i = 0; i < surfaces.getCount(); i++ )
		{
			surfaces[i]->setWindowedMode();
		}
	}
}

void App::stop()
{
	running = false;
}

void App::externalTick()
{
	internalTick();
}

bool App::wasMousePressed( MouseCode code, Panel * )
{
	return mousePressed[code];
}

bool App::wasMouseDoublePressed( MouseCode code, Panel * )
{
	return mouseDoublePressed[code];
}

bool App::isMouseDown( MouseCode code, Panel * )
{
	return mouseDown[code];
}

bool App::wasMouseReleased( MouseCode code, Panel * )
{
	return mouseReleased[code];
}

bool App::wasKeyPressed( KeyCode code, Panel *p )
{
	if( p && keyFocusPanel != p )
		return false;

	return keyPressed[code];
}

bool App::isKeyDown( KeyCode code, Panel *p )
{
	if( p && keyFocusPanel != p )
		return false;

	return keyDown[code];
}

bool App::wasKeyTyped( KeyCode code, Panel *p )
{
	if( p && keyFocusPanel != p )
		return false;

	return keyTyped[code];
}

bool App::wasKeyReleased( KeyCode code, Panel *p )
{
	if( p && keyFocusPanel != p )
		return false;

	return keyReleased[code];
}

void App::addTickSignal( TickSignal *s )
{
	tickSignals.putElement( s );
}

void App::setCursorPos( int x, int y )
{
	// stub
}

void App::getCursorPos( int &x, int &y )
{
	surfaces[0]->GetMousePos( x, y );
}

void App::setMouseCapture( Panel *p )
{
	if( p )
		p->surfaceBase->enableMouseCapture( true );
	else if( mouseCapturePanel )
		mouseCapturePanel->surfaceBase->enableMouseCapture( false );

	mouseCapturePanel = p;
}

void App::setMouseArena( int x1, int y1, int x2, int y2, bool enable )
{
	setMouseArena( nullptr );
	internalSetMouseArena( x1, y1, x2, y2, enable );
}

void App::setMouseArena( Panel *p )
{
	mouseArenaPanel = p;
}

void App::requestFocus( Panel *p )
{
	keyFocusPanelRequested = p;
}

Panel *App::getFocus()
{
	return keyFocusPanel;
}

void App::repaintAll()
{
	for( int i = 0; i < surfaces.getCount(); i++ )
	{
		SurfaceBase* s = surfaces[i];
		Panel* p = s->getPanel();
		p->repaintAll();
		s->invalidate( p );
	}
}

void App::setScheme( Scheme *sc )
{
	if( !sc )
		return;

	scheme = sc;
	repaintAll();
}

Scheme *App::getScheme()
{
	return scheme;
}

void App::enableBuildMode()
{
	buildModeRequested = true;
}

char App::getKeyCodeChar( KeyCode code, bool shift )
{
	return staticKeyTrans[code][ shift ? 1 : 0 ];
}

void App::getKeyCodeText( KeyCode code, char *dst, int size )
{
	vgui_strcpy( dst, size, staticKeyTrans[code] + 2 );
}

int App::getClipboardTextCount()
{
	return 0;
}

void App::setClipboardText( const char*, int )
{
}

int App::getClipboardText( int, char*, int )
{
	return 0;
}

void App::reset()
{
	mouseArenaPanel = nullptr;
	tickSignals.removeAll();
	keyFocusPanel = mouseFocusPanel = mouseCapturePanel = keyFocusPanelRequested = nullptr;
	buildModeEnabled = buildModeRequested = false;
	Font_Reset();
	setScheme( new Scheme( ));
}

bool App::setRegistryString( const char *, const char * )
{
	return false;
}

bool App::getRegistryString( const char *, char *, int )
{
	return false;
}

bool App::setRegistryInteger( const char *, int )
{
	return false;
}

bool App::getRegistryInteger( const char *, int & )
{
	return false;
}

void App::setCursorOveride( Cursor *c )
{
	cursorOverride = c;
}

Cursor *App::getCursorOveride()
{
	return cursorOverride;
}

void App::setMinimumTickMillisInterval( int i )
{
	minimumTickMillisInterval = i;
}

void App::run()
{
	running = true;
	do
	{
		internalTick();
	} while( running );
	setMouseArena( 0, 0, 0, 0, false );
}

void App::internalCursorMoved( int x, int y, SurfaceBase *s )
{
	s->getPanel()->localToScreen( x, y );

	if( !buildModeEnabled )
	{
		updateMouseFocus( x, y, s );
		if( mouseFocusPanel )
			mouseFocusPanel->internalCursorMoved( x, y );
	}
}

void App::internalMousePressed( MouseCode code, SurfaceBase * )
{
	mousePressed[code] = true;
	mouseDown[code] = true;

	if( !buildModeEnabled && mouseFocusPanel )
		mouseFocusPanel->internalMousePressed( code );
}

void App::internalMouseDoublePressed( MouseCode code, SurfaceBase * )
{
	mouseDoublePressed[code] = true;

	if( !buildModeEnabled && mouseFocusPanel )
		mouseFocusPanel->internalMouseDoublePressed( code );
}

void App::internalMouseReleased( MouseCode code, SurfaceBase * )
{
	mouseReleased[code] = true;
	mouseDown[code] = false;

	if( !buildModeEnabled && mouseFocusPanel )
		mouseFocusPanel->internalMouseReleased( code );
}

void App::internalMouseWheeled( int i, SurfaceBase * )
{
	if( !buildModeEnabled && mouseFocusPanel )
		mouseFocusPanel->internalMouseWheeled( i );
}

void App::internalKeyPressed( KeyCode code, SurfaceBase * )
{
	if( code >= KEY_0 && code < KEY_LAST )
	{
		keyPressed[code] = true;
		keyDown[code] = true;

		if( !buildModeEnabled && keyFocusPanel )
			keyFocusPanel->internalKeyPressed( code );
	}
}

void App::internalKeyTyped( KeyCode code, SurfaceBase * )
{
	if( code >= KEY_0 && code < KEY_LAST )
	{
		keyTyped[code] = true;

		if( !buildModeEnabled && keyFocusPanel )
			keyFocusPanel->internalKeyTyped( code );
	}
}

void App::internalKeyReleased( KeyCode code, SurfaceBase * )
{
	if( code >= KEY_0 && code < KEY_LAST )
	{
		keyReleased[code] = true;
		keyDown[code] = false;

		if( !buildModeEnabled && keyFocusPanel )
			keyFocusPanel->internalKeyReleased( code );
	}
}

void App::init()
{
	externalMainLoop = false;
	running = false;
	keyFocusPanel = oldMouseFocusPanel = mouseCapturePanel = keyFocusPanelRequested = nullptr;
	singleton = this;
	scheme = new Scheme();
	mouseArenaPanel = nullptr;
	buildModeEnabled = buildModeRequested = false;
	cursorOverride = nullptr;
	nextTickMillis = getTimeMillis();
	minimumTickMillisInterval = 50;

	memset( mousePressed, 0, sizeof( mousePressed ));
	memset( mouseDoublePressed, 0, sizeof( mouseDoublePressed ));
	memset( mouseReleased, 0, sizeof( mouseReleased ));
	memset( mouseDown, 0, sizeof( mouseDown ));
	memset( keyPressed, 0, sizeof( keyPressed ));
	memset( keyTyped, 0, sizeof( keyTyped ));
	memset( keyReleased, 0, sizeof( keyReleased ));
	memset( keyDown, 0, sizeof( keyDown ));
}

void App::updateMouseFocus( int x, int y, SurfaceBase *s )
{
	if( mouseCapturePanel )
	{
		setMouseFocus( mouseCapturePanel );
	}
	else if( s->isWithin( x, y ))
	{
		Panel *p = s->getPanel()->isWithinTraverse( x, y );
		if( p )
			setMouseFocus( p );
	}
}

void App::setMouseFocus( Panel *p )
{
	if( p != mouseFocusPanel )
	{
		oldMouseFocusPanel = mouseFocusPanel;
		mouseFocusPanel = p;

		if( oldMouseFocusPanel )
			oldMouseFocusPanel->internalCursorExited();

		if( mouseFocusPanel )
			mouseFocusPanel->internalCursorEntered();
	}
}

void App::surfaceBaseCreated( SurfaceBase *s )
{
	surfaces.putElement( s );
}

void App::surfaceBaseDeleted( SurfaceBase *s )
{
	surfaces.removeElement( s );
	mouseFocusPanel = keyFocusPanel = mouseCapturePanel = nullptr;
}

void App::internalTick()
{
	if( getTimeMillis() < nextTickMillis )
		return;

	platTick();

	int x, y;
	getCursorPos( x, y );

	bool found = false;
	for( int i = 0; i < surfaces.getCount(); i++ )
	{
		SurfaceBase *s = surfaces[i];

		updateMouseFocus( x, y, s );

		if( s->isWithin( x, y ))
			found = true;

		s->setEmulatedCursorPos( x, y );
	}

	if( !found )
		setMouseFocus( nullptr );

	if( mouseFocusPanel )
		mouseFocusPanel->internalSetCursor();

	for( int i = 0; i < tickSignals.getCount(); i++ )
		tickSignals[i]->ticked();

	if( keyFocusPanel )
	{
		keyFocusPanel->internalKeyFocusTicked();
	}
	else
	{
		surfaces[0]->getPanel()->requestFocus();
		if( keyFocusPanel )
			keyFocusPanel->internalKeyFocusTicked();
	}

	memset( mousePressed, 0, sizeof( mousePressed ));
	memset( mouseDoublePressed, 0, sizeof( mouseDoublePressed ));
	memset( mouseReleased, 0, sizeof( mouseReleased ));
	memset( keyPressed, 0, sizeof( keyPressed ));
	memset( keyTyped, 0, sizeof( keyTyped ));
	memset( keyReleased, 0, sizeof( keyReleased ));

	found = false;
	for( int i = 0; i < surfaces.getCount(); i++ )
	{
		if( surfaces[i]->hasFocus() )
		{
			found = true;
			break;
		}
	}

	if( !found )
		keyFocusPanelRequested = nullptr;

	if( keyFocusPanel != keyFocusPanelRequested )
	{
		if( keyFocusPanel )
		{
			keyFocusPanel->internalFocusChanged( true );
			keyFocusPanel->repaint();
		}

		if( keyFocusPanelRequested )
		{
			keyFocusPanelRequested->internalFocusChanged( false );
			keyFocusPanelRequested->repaint();
		}
	}

	keyFocusPanel = keyFocusPanelRequested;
	buildModeEnabled = buildModeRequested;

	for( int i = 0; i < surfaces.getCount(); i++ )
	{
		surfaces[i]->getPanel()->solveTraverse();
		surfaces[i]->applyChanges();
	}

	if( mouseArenaPanel )
	{
		SurfaceBase *s = mouseArenaPanel->getSurfaceBase();
		if( s )
		{
			s->getPanel()->getPos( x, y );

			int extents[4];
			mouseArenaPanel->getAbsExtents( extents[0], extents[1], extents[2], extents[3] );
			internalSetMouseArena(
				x + extents[0],
				y + extents[1],
				x + extents[2],
				y + extents[3], true );
		}
	}

	nextTickMillis = getTimeMillis() + minimumTickMillisInterval;
}
