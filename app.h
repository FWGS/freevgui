// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_APP_H
#define VGUI_APP_H

#include "vgui.h"
#include "input.h"

namespace vgui
{
class Panel;
class Scheme;
class SurfaceBase;
class TickSignal;

class CLASSEXPORT App
{
	friend class SurfaceBase;
public:
	App( bool externalMain );
	App();

	static App *getInstance();
	virtual void start();
	virtual void stop();
	virtual void externalTick();
	virtual bool wasMousePressed( MouseCode code, Panel *p );
	virtual bool wasMouseDoublePressed( MouseCode code, Panel *p );
	virtual bool isMouseDown( MouseCode code, Panel *p );
	virtual bool wasMouseReleased( MouseCode code, Panel *p );
	virtual bool wasKeyPressed( KeyCode code, Panel *p );
	virtual bool isKeyDown( KeyCode code, Panel *p );
	virtual bool wasKeyTyped( KeyCode code, Panel *p );
	virtual bool wasKeyReleased( KeyCode code, Panel *p );
	virtual void addTickSignal( TickSignal *s );
	virtual void setCursorPos( int x, int y );
	virtual void getCursorPos( int &x, int &y );
	virtual void setMouseCapture( Panel *p );
	virtual void setMouseArena( int x1, int y1, int x2, int y2, bool enable );
	virtual void setMouseArena( Panel *p );
	virtual void requestFocus( Panel *p );
	virtual Panel *getFocus();
	virtual void repaintAll();
	virtual void setScheme( Scheme *sc );
	virtual Scheme *getScheme();
	virtual void enableBuildMode();
	virtual long int getTimeMillis(); // platform-dependent
	virtual char getKeyCodeChar( KeyCode, bool );
	virtual void getKeyCodeText( KeyCode, char *, int );
	virtual int getClipboardTextCount();
	virtual void setClipboardText( const char *, int );
	virtual int getClipboardText( int, char *, int );
	virtual void reset();
	virtual void internalSetMouseArena( int, int, int, int, bool ); // platform-dependent
	virtual bool setRegistryString( const char *, const char * );
	virtual bool getRegistryString( const char *, char *, int );
	virtual bool setRegistryInteger( const char *, int );
	virtual bool getRegistryInteger( const char *, int & );
	virtual void setCursorOveride( Cursor *c ); // NOTE: overide, with single r
	virtual Cursor *getCursorOveride();
	virtual void setMinimumTickMillisInterval( int i );
	virtual void main( int, char** ) = 0;
	virtual void run();
	virtual void internalCursorMoved( int x, int y, SurfaceBase *s );
	virtual void internalMousePressed( MouseCode code, SurfaceBase * );
	virtual void internalMouseDoublePressed( MouseCode code, SurfaceBase * );
	virtual void internalMouseReleased( MouseCode code, SurfaceBase * );
	virtual void internalMouseWheeled( int i, SurfaceBase * );
	virtual void internalKeyPressed( KeyCode code, SurfaceBase * );
	virtual void internalKeyTyped( KeyCode code, SurfaceBase * );
	virtual void internalKeyReleased( KeyCode code, SurfaceBase * );

private:
	virtual void init();
	virtual void updateMouseFocus( int x, int y, SurfaceBase *s );
	virtual void setMouseFocus( Panel *p );

protected:
	virtual void surfaceBaseCreated( SurfaceBase *s );
	virtual void surfaceBaseDeleted( SurfaceBase *s );
	virtual void platTick();
	virtual void internalTick();

	static App *_instance;
	bool _running, _externalMain;
	Dar<SurfaceBase*> _surfaceBaseDar;
	Panel *_keyFocus;
	Panel *_oldMouseFocus;
	Panel *_mouseFocus;
	Panel *_mouseCapture;
	Panel *_wantedKeyFocus;
	bool _mousePressed[MOUSE_LAST];
	bool _mouseDoublePressed[MOUSE_LAST];
	bool _mouseDown[MOUSE_LAST];
	bool _mouseReleased[MOUSE_LAST];
	bool _keyPressed[KEY_LAST];
	bool _keyTyped[KEY_LAST];
	bool _keyDown[KEY_LAST];
	bool _keyReleased[KEY_LAST];
	Dar<TickSignal*> _tickSignalDar;
	Scheme *_scheme;
	bool _buildMode, _wantedBuildMode;
	Panel *_mouseArenaPanel;
	Cursor *_cursor[Cursor::DC_LAST];
	Cursor *_cursorOveride;
private:
	long int _nextTickMillis;
	long int _minimumTickMillisInterval;
};
CHECK_STRUCT_SIZE( App, 556, 672, 664 );
}

#endif // VGUI_APP_H
