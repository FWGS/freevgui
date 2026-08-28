// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_SURFACE_H
#define VGUI_SURFACE_H

#include "vgui.h"

namespace vgui
{
class App;
class Cursor;
class Font;
class ImagePanel;
class Panel;
class SurfacePlat;

class CLASSEXPORT SurfaceBase
{
	friend class App;
	friend class Panel;
public:
	SurfaceBase( Panel *embeddedPanel );
	virtual Panel *getPanel();
	virtual void requestSwap();
	virtual void resetModeInfo();
	virtual int getModeInfoCount();
	virtual bool getModeInfo( int i, int &w, int &h, int &bpp );
	virtual App *getApp();
	virtual void setEmulatedCursorVisible( bool visible );
	virtual void setEmulatedCursorPos( int x, int y );
	virtual void setTitle( const char * ) = 0;
	virtual bool setFullscreenMode( int, int, int ) = 0;
	virtual void setWindowedMode() = 0;
	virtual void setAsTopMost( bool ) = 0;
	virtual void createPopup( Panel * ) = 0;
	virtual bool hasFocus() = 0;
	virtual bool isWithin( int, int ) = 0;
	virtual int createNewTextureID() = 0;
	virtual void GetMousePos( int &, int & ) = 0;
protected:
	bool pendingSwap;
	App *app;
	Panel *rootPanel;
	Dar<char *> modes;
	ImagePanel *softwareCursor;
	Cursor *cursor;

	~SurfaceBase();

	virtual void addModeInfo( int w, int h, int bpp );
	virtual void drawSetColor( int, int, int, int ) = 0;
	virtual void drawFilledRect( int, int, int, int ) = 0;
	virtual void drawOutlinedRect( int, int, int, int ) = 0;
	virtual void drawSetTextFont( Font * ) = 0;
	virtual void drawSetTextColor( int, int, int, int ) = 0;
	virtual void drawSetTextPos( int, int ) = 0;
	virtual void drawPrintText( const char *, int ) = 0;
	virtual void drawSetTextureRGBA( int, const char *, int, int ) = 0;
	virtual void drawSetTexture( int ) = 0;
	virtual void drawTexturedRect( int, int, int, int ) = 0;
	virtual void invalidate( Panel * ) = 0;
	virtual void enableMouseCapture( bool ) = 0;
	virtual void setCursor( Cursor * ) = 0;
	virtual void swapBuffers() = 0;
	virtual void pushMakeCurrent( Panel *, bool ) = 0;
	virtual void popMakeCurrent( Panel * ) = 0;
	virtual void applyChanges() = 0;
};
CHECK_STRUCT_SIZE( SurfaceBase, 36, 64, 64 );

class CLASSEXPORT Surface : public SurfaceBase
{
public:
	Surface( Panel *p );

	virtual void setTitle( const char * ) override; // platform-dependent
	virtual bool setFullscreenMode( int w, int h, int bpp ) override; // platform-dependent
	virtual void setWindowedMode() override; // platform-dependent
	virtual void setAsTopMost( bool ) override; // platform-dependent
	virtual int getModeInfoCount() override; // platform-dependent
	virtual void createPopup( Panel * ) override;
	virtual bool hasFocus() override; // platform-dependent
	virtual bool isWithin( int, int ) override; // platform-dependent
	virtual void GetMousePos( int &x, int &y ) override; // platform-dependent

protected:
	virtual int createNewTextureID() override; // platform-dependent
	virtual void drawSetColor( int, int, int, int ) override; // platform-dependent
	virtual void drawFilledRect( int, int, int, int ) override; // platform-dependent
	virtual void drawOutlinedRect( int, int, int, int ) override; // platform-dependent
	virtual void drawSetTextFont( Font * ) override; // platform-dependent
	virtual void drawSetTextColor( int, int, int, int ) override; // platform-dependent
	virtual void drawSetTextPos( int, int ) override; // platform-dependent
	virtual void drawPrintText( const char *, int ) override; // platform-dependent
	virtual void drawSetTextureRGBA( int, const char *, int, int ) override; // platform-dependent
	virtual void drawSetTexture( int ) override; // platform-dependent
	virtual void drawTexturedRect( int, int, int, int ) override; // platform-dependent
	virtual void invalidate( Panel * ) override; // platform-dependent
	virtual bool createPlat(); // platform-dependent
	virtual bool recreateContext(); // platform-dependent
	virtual void enableMouseCapture( bool ) override; // platform-dependent
	virtual void setCursor( Cursor * ) override; // platform-dependent
	virtual void swapBuffers() override; // platform-dependent
	virtual void pushMakeCurrent( Panel *, bool ) override; // platform-dependent
	virtual void popMakeCurrent( Panel * ) override; // platform-dependent
	virtual void applyChanges() override; // platform-dependent

	SurfacePlat *impl;
	// fix -Werror=shadow
	bool ownPendingSwap;
	Panel *ownRootPanel;
	Dar<char *> ownModes;
};
CHECK_STRUCT_SIZE( Surface, 60, 104, 104 );
}

#endif // VGUI_SURFACE_H
