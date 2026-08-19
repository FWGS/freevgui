// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_XASH3D_SUPPORT_H
#define VGUI_XASH3D_SUPPORT_H

#include "vgui.h"
#include "app.h"
#include "input.h"
#include "panel.h"
#include "surface.h"

#include "vgui_api.h"

namespace vgui
{
class Font;
class XashSurface;

extern vguiapi_t *g_engine; // the engine half of the API. The structure is owned by the engine, we only keep the pointer

// the root panel the game client library parents its own panels to, and the surface that draws it
extern Panel *g_rootPanel;
extern XashSurface *g_surface;

// the engine 2D drawing path has no scissor test of its own, so we clip rectangles ourselves
class Scissor
{
public:
	Scissor();

	void enable();
	void disable();
	void setRect( int x0, int y0, int x1, int y1 );

	// intersects the quad with the current rectangle, interpolating the texture coordinates
	// of the clipped corners. False means the quad is entirely outside and must not be drawn
	bool clip( const vpoint_t &topLeft, const vpoint_t &bottomRight, vpoint_t &clippedTopLeft, vpoint_t &clippedBottomRight ) const;

private:
	bool enabled;
	int  rect[4];
};

extern Scissor g_scissor;

// input entry points the engine calls (input.cpp)
void XashMouse( VGUI_MouseAction action, int code );
void XashKey( VGUI_KeyAction action, VGUI_KeyCode code );
void XashMouseMove( int x, int y );
void XashTextInput( const char *text );

class XashApp : public App
{
public:
	XashApp() : App( true ) { }
	virtual void main( int, char ** ) override { }
};

class XashSurface : public SurfaceBase
{
public:
	XashSurface( Panel *embeddedPanel );
	~XashSurface();

	// the engine owns the window, all of these are no-op
	virtual void setTitle( const char * ) override;
	virtual bool setFullscreenMode( int, int, int ) override;
	virtual void setWindowedMode() override;
	virtual void setAsTopMost( bool ) override;
	virtual void createPopup( Panel * ) override;
	virtual bool hasFocus() override;
	virtual bool isWithin( int, int ) override;
	virtual int createNewTextureID() override;
	virtual void GetMousePos( int &x, int &y ) override;

protected:
	virtual void drawSetColor( int r, int g, int b, int a ) override;
	virtual void drawFilledRect( int x0, int y0, int x1, int y1 ) override;
	virtual void drawOutlinedRect( int x0, int y0, int x1, int y1 ) override;
	virtual void drawSetTextFont( Font *font ) override;
	virtual void drawSetTextColor( int r, int g, int b, int a ) override;
	virtual void drawSetTextPos( int x, int y ) override;
	virtual void drawPrintText( const char *text, int textLen ) override;
	virtual void drawSetTextureRGBA( int id, const char *rgba, int wide, int tall ) override;
	virtual void drawSetTexture( int id ) override;
	virtual void drawTexturedRect( int x0, int y0, int x1, int y1 ) override;
	virtual void invalidate( Panel * ) override;
	virtual void enableMouseCapture( bool ) override;
	virtual void setCursor( Cursor *cursor ) override;
	virtual void swapBuffers() override;
	virtual void pushMakeCurrent( Panel *panel, bool useInsets ) override;
	virtual void popMakeCurrent( Panel *panel ) override;
	virtual void applyChanges() override;

private:
	enum
	{
		FONT_PAGE_SIZE  = 512,
		MAX_FONT_PAGES  = 8, // per font
		MAX_PAINT_STACK = 16,
		DEFAULT_COLOR   = 7,
	};

	// vgui doesn't make font atlas, we handle that ourselves
	struct FontCache
	{
		int           id;                   // Font::getId()
		int           pageCount;
		unsigned char page[256];            // atlas page each glyph landed on
		int           texture[MAX_FONT_PAGES];
		float         coord[256][4];        // s0, t0, s1, t1
		int           generation;
	};

	struct PaintState
	{
		Panel *panel; // for validation only
		int translate[2];
		int clip[4];
	};

	FontCache *buildFontCache( Font *font );
	void uploadFontPage( FontCache *cache, int index, const unsigned char *pixels );
	void makeCurrent( const PaintState *ps );

	int   drawColor[4];
	int   textColor[4];
	int   textPos[2];
	int   translate[2];
	Font *currentFont;

	FontCache  *currentFontCache;
	static Dar<FontCache *> fontCacheDar;
	static int fontGeneration;

	// ^N colour escape state, carried across drawPrintText calls
	bool sawCaret;
	int  colorIndex;

	PaintState paintStack[MAX_PAINT_STACK];
	int paintStackPos;
};
}

#endif // VGUI_XASH3D_SUPPORT_H
