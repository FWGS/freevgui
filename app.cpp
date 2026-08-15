// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "app.h"
#include "vgui_internal.h"
#include "surface.h"
#include "signals.h"
#include "panel.h"
#include "font.h"

using namespace vgui;

App* App::_instance = nullptr;
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
	_externalMain = externalMain;
}

App::App() : App( false ) { }

App *App::getInstance()
{
	return _instance;
}

void App::start()
{
	if( !_externalMain )
	{
		run();
		for( int i = 0; i < _surfaceBaseDar.getCount(); i++ )
		{
			_surfaceBaseDar[i]->setWindowedMode();
		}
	}
}

void App::stop()
{
	_running = false;
}

void App::externalTick()
{
	internalTick();
}

bool App::wasMousePressed( MouseCode code, Panel * )
{
	return _mousePressed[code];
}

bool App::wasMouseDoublePressed( MouseCode code, Panel * )
{
	return _mouseDoublePressed[code];
}

bool App::isMouseDown( MouseCode code, Panel * )
{
	return _mouseDown[code];
}

bool App::wasMouseReleased( MouseCode code, Panel * )
{
	return _mouseReleased[code];
}

bool App::wasKeyPressed( KeyCode code, Panel *p )
{
	if( p && _keyFocus != p )
		return false;

	return _keyPressed[code];
}

bool App::isKeyDown( KeyCode code, Panel *p )
{
	if( p && _keyFocus != p )
		return false;

	return _keyDown[code];
}

bool App::wasKeyTyped( KeyCode code, Panel *p )
{
	if( p && _keyFocus != p )
		return false;

	return _keyTyped[code];
}

bool App::wasKeyReleased( KeyCode code, Panel *p )
{
	if( p && _keyFocus != p )
		return false;

	return _keyReleased[code];
}

void App::addTickSignal( TickSignal *s )
{
	_tickSignalDar.putElement( s );
}

void App::setCursorPos( int x, int y )
{
	// stub
}

void App::getCursorPos( int &x, int &y )
{
	_surfaceBaseDar[0]->GetMousePos( x, y );
}

void App::setMouseCapture( Panel *p )
{
	if( p )
		p->_surfaceBase->enableMouseCapture( true );
	else if( _mouseCapture )
		_mouseCapture->_surfaceBase->enableMouseCapture( false );

	_mouseCapture = p;
}

void App::setMouseArena( int x1, int y1, int x2, int y2, bool enable )
{
	setMouseArena( nullptr );
	internalSetMouseArena( x1, y1, x2, y2, enable );
}

void App::setMouseArena( Panel *p )
{
	_mouseArenaPanel = p;
}

void App::requestFocus( Panel *p )
{
	_wantedKeyFocus = p;
}

Panel *App::getFocus()
{
	return _keyFocus;
}

void App::repaintAll()
{
	for( int i = 0; i < _surfaceBaseDar.getCount(); i++ )
	{
		SurfaceBase* s = _surfaceBaseDar[i];
		Panel* p = s->getPanel();
		p->repaintAll();
		s->invalidate( p );
	}
}

void App::setScheme( Scheme *sc )
{
	if( !sc )
		return;

	_scheme = sc;
	repaintAll();
}

Scheme *App::getScheme()
{
	return _scheme;
}

void App::enableBuildMode()
{
	_wantedBuildMode = true;
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
	_mouseArenaPanel = nullptr;
	_tickSignalDar.removeAll();
	_keyFocus = _mouseFocus = _mouseCapture = _wantedKeyFocus = nullptr;
	_buildMode = _wantedBuildMode = false;
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
	_cursorOveride = c;
}

Cursor *App::getCursorOveride()
{
	return _cursorOveride;
}

void App::setMinimumTickMillisInterval( int i )
{
	_minimumTickMillisInterval = i;
}

void App::run()
{
	_running = true;
	do
	{
		internalTick();
	} while( _running );
	setMouseArena( 0, 0, 0, 0, false );
}

void App::internalCursorMoved( int x, int y, SurfaceBase *s )
{
	s->getPanel()->localToScreen( x, y );

	if( !_buildMode )
	{
		updateMouseFocus( x, y, s );
		if( _mouseFocus )
			_mouseFocus->internalCursorMoved( x, y );
	}
}

void App::internalMousePressed( MouseCode code, SurfaceBase * )
{
	_mousePressed[code] = true;
	_mouseDown[code] = true;

	if( !_buildMode && _mouseFocus )
		_mouseFocus->internalMousePressed( code );
}

void App::internalMouseDoublePressed( MouseCode code, SurfaceBase * )
{
	_mouseDoublePressed[code] = true;

	if( !_buildMode && _mouseFocus )
		_mouseFocus->internalMouseDoublePressed( code );
}

void App::internalMouseReleased( MouseCode code, SurfaceBase * )
{
	_mouseReleased[code] = true;
	_mouseDown[code] = false;

	if( !_buildMode && _mouseFocus )
		_mouseFocus->internalMouseReleased( code );
}

void App::internalMouseWheeled( int i, SurfaceBase * )
{
	if( !_buildMode && _mouseFocus )
		_mouseFocus->internalMouseWheeled( i );
}

void App::internalKeyPressed( KeyCode code, SurfaceBase * )
{
	if( code >= KEY_0 && code < KEY_LAST )
	{
		_keyPressed[code] = true;
		_keyDown[code] = true;

		if( !_buildMode && _keyFocus )
			_keyFocus->internalKeyPressed( code );
	}
}

void App::internalKeyTyped( KeyCode code, SurfaceBase * )
{
	if( code >= KEY_0 && code < KEY_LAST )
	{
		_keyTyped[code] = true;

		if( !_buildMode && _keyFocus )
			_keyFocus->internalKeyTyped( code );
	}
}

void App::internalKeyReleased( KeyCode code, SurfaceBase * )
{
	if( code >= KEY_0 && code < KEY_LAST )
	{
		_keyReleased[code] = true;
		_keyDown[code] = false;

		if( !_buildMode && _keyFocus )
			_keyFocus->internalKeyReleased( code );
	}
}

void App::init()
{
	_externalMain = false;
	_running = false;
	_keyFocus = _oldMouseFocus = _mouseCapture = _wantedKeyFocus = nullptr;
	_instance = this;
	_scheme = new Scheme();
	_mouseArenaPanel = nullptr;
	_buildMode = _wantedBuildMode = false;
	_cursorOveride = nullptr;
	_nextTickMillis = getTimeMillis();
	_minimumTickMillisInterval = 50;

	memset( _mousePressed, 0, sizeof( _mousePressed ));
	memset( _mouseDoublePressed, 0, sizeof( _mouseDoublePressed ));
	memset( _mouseReleased, 0, sizeof( _mouseReleased ));
	memset( _mouseDown, 0, sizeof( _mouseDown ));
	memset( _keyPressed, 0, sizeof( _keyPressed ));
	memset( _keyTyped, 0, sizeof( _keyTyped ));
	memset( _keyReleased, 0, sizeof( _keyReleased ));
	memset( _keyDown, 0, sizeof( _keyDown ));
}

void App::updateMouseFocus( int x, int y, SurfaceBase *s )
{
	if( _mouseCapture )
	{
		setMouseFocus( _mouseCapture );
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
	if( p != _mouseFocus )
	{
		_oldMouseFocus = _mouseFocus;
		_mouseFocus = p;

		if( _oldMouseFocus )
			_oldMouseFocus->internalCursorExited();

		if( _mouseFocus )
			_mouseFocus->internalCursorEntered();
	}
}

void App::surfaceBaseCreated( SurfaceBase *s )
{
	_surfaceBaseDar.putElement( s );
}

void App::surfaceBaseDeleted( SurfaceBase *s )
{
	_surfaceBaseDar.removeElement( s );
	_mouseFocus = _keyFocus = _mouseCapture = nullptr;
}

void App::internalTick()
{
	if( getTimeMillis() < _nextTickMillis )
		return;

	platTick();

	int x, y;
	getCursorPos( x, y );

	bool found = false;
	for( int i = 0; i < _surfaceBaseDar.getCount(); i++ )
	{
		SurfaceBase *s = _surfaceBaseDar[i];

		updateMouseFocus( x, y, s );

		if( s->isWithin( x, y ))
			found = true;

		s->setEmulatedCursorPos( x, y );
	}

	if( !found )
		setMouseFocus( nullptr );

	if( _mouseFocus )
		_mouseFocus->internalSetCursor();

	for( int i = 0; i < _tickSignalDar.getCount(); i++ )
		_tickSignalDar[i]->ticked();

	if( _keyFocus )
	{
		_keyFocus->internalKeyFocusTicked();
	}
	else
	{
		_surfaceBaseDar[0]->getPanel()->requestFocus();
		if( _keyFocus )
			_keyFocus->internalKeyFocusTicked();
	}

	memset( _mousePressed, 0, sizeof( _mousePressed ));
	memset( _mouseDoublePressed, 0, sizeof( _mouseDoublePressed ));
	memset( _mouseReleased, 0, sizeof( _mouseReleased ));
	memset( _keyPressed, 0, sizeof( _keyPressed ));
	memset( _keyTyped, 0, sizeof( _keyTyped ));
	memset( _keyReleased, 0, sizeof( _keyReleased ));

	found = false;
	for( int i = 0; i < _surfaceBaseDar.getCount(); i++ )
	{
		if( _surfaceBaseDar[i]->hasFocus() )
		{
			found = true;
			break;
		}
	}

	if( !found )
		_wantedKeyFocus = nullptr;

	if( _keyFocus != _wantedKeyFocus )
	{
		if( _keyFocus )
		{
			_keyFocus->internalFocusChanged( true );
			_keyFocus->repaint();
		}

		if( _wantedKeyFocus )
		{
			_wantedKeyFocus->internalFocusChanged( false );
			_wantedKeyFocus->repaint();
		}
	}

	_keyFocus = _wantedKeyFocus;
	_buildMode = _wantedBuildMode;

	for( int i = 0; i < _surfaceBaseDar.getCount(); i++ )
	{
		_surfaceBaseDar[i]->getPanel()->solveTraverse();
		_surfaceBaseDar[i]->applyChanges();
	}

	if( _mouseArenaPanel )
	{
		SurfaceBase *s = _mouseArenaPanel->getSurfaceBase();
		if( s )
		{
			s->getPanel()->getPos( x, y );

			int extents[4];
			_mouseArenaPanel->getAbsExtents( extents[0], extents[1], extents[2], extents[3] );
			internalSetMouseArena(
				x + extents[0],
				y + extents[1],
				x + extents[2],
				y + extents[3], true );
		}
	}

	_nextTickMillis = getTimeMillis() + _minimumTickMillisInterval;
}
