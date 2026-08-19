// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "vgui_internal.h"
#include "font.h"
#include "support.h"

using namespace vgui;

Dar<XashSurface::FontCache *> XashSurface::fontCacheDar;
int XashSurface::fontGeneration = 0;

static bool Q_isspace( int ch ) // locale independent
{
	return ch == ' ' || ( ch >= '\t' && ch <= '\r' );
}

XashSurface::XashSurface( Panel *embeddedPanel ) :
	SurfaceBase( embeddedPanel ),
	currentFont( nullptr ), currentFontCache( nullptr ),
	sawCaret( false ), colorIndex( DEFAULT_COLOR ), paintStackPos( 0 )
{
	// don't forget alpha is inverted in VGUI... 255 means fully transparent
	for( int i = 0; i < 4; i++ )
		drawColor[i] = textColor[i] = 255;

	textPos[0] = textPos[1] = 0;
	translate[0] = translate[1] = 0;

	// DrawInit/DrawShutdown cleans up textures
	for( int i = 0; i < fontCacheDar.getCount(); i++ )
		delete fontCacheDar[i];

	fontCacheDar.removeAll();
	fontGeneration++;
}

XashSurface::~XashSurface()
{
	g_engine->DrawShutdown();
}

void XashSurface::setTitle( const char * )
{
	// no-op in the game engine context
}

bool XashSurface::setFullscreenMode( int, int, int )
{
	// no-op in the game engine context
	return false;
}

void XashSurface::setWindowedMode()
{
	// no-op in the game engine context
}

void XashSurface::setAsTopMost( bool )
{
	// no-op in the game engine context
}

void XashSurface::createPopup( Panel * )
{
	// no-op in the game engine context
}

bool XashSurface::hasFocus()
{
	// no-op in the game engine context
	return true;
}

bool XashSurface::isWithin( int, int )
{
	// the surface covers the whole virtual screen
	return true;
}

int XashSurface::createNewTextureID()
{
	return g_engine->GenerateTexture();
}

void XashSurface::GetMousePos( int &x, int &y )
{
	g_engine->GetCursorPos( &x, &y );
}

void XashSurface::invalidate( Panel * )
{
}

void XashSurface::enableMouseCapture( bool )
{
	// the engine delivers all input to us regardless, so there is nothing to capture
}

void XashSurface::setCursor( Cursor *cursor )
{
	_currentCursor = cursor;

	// bitmap cursors report DC_USER, which the engine platform layer hides just like DC_NONE
	if( cursor )
		g_engine->CursorSelect( (VGUI_DefaultCursor)cursor->getDefaultCursor());
}

void XashSurface::swapBuffers()
{
	// no-op in the game engine context
}

void XashSurface::applyChanges()
{
}

void XashSurface::drawSetColor( int r, int g, int b, int a )
{
	drawColor[0] = r;
	drawColor[1] = g;
	drawColor[2] = b;
	drawColor[3] = a;
}

void XashSurface::drawSetTextColor( int r, int g, int b, int a )
{
	textColor[0] = r;
	textColor[1] = g;
	textColor[2] = b;
	textColor[3] = a;
}

void XashSurface::drawSetTextPos( int x, int y )
{
	textPos[0] = x;
	textPos[1] = y;
}

void XashSurface::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	if( drawColor[3] >= 255 ) // fully transparent
		return;

	vpoint_t topLeft, bottomRight, clippedTopLeft, clippedBottomRight;

	topLeft.point[0] = x0 + translate[0];
	topLeft.point[1] = y0 + translate[1];
	bottomRight.point[0] = x1 + translate[0];
	bottomRight.point[1] = y1 + translate[1];
	topLeft.coord[0] = topLeft.coord[1] = 0.0f;
	bottomRight.coord[0] = bottomRight.coord[1] = 0.0f;

	if( !g_scissor.clip( topLeft, bottomRight, clippedTopLeft, clippedBottomRight ))
		return;

	g_engine->SetupDrawingRect( drawColor );
	g_engine->EnableTexture( false );
	g_engine->DrawQuad( &clippedTopLeft, &clippedBottomRight );
	g_engine->EnableTexture( true ); // the flag is engine global, restore it
}

void XashSurface::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	if( drawColor[3] >= 255 )
		return;

	drawFilledRect( x0, y0, x1, y0 + 1 );
	drawFilledRect( x0, y1 - 1, x1, y1 );

	// inset vertically so the corners aren't drawn twice
	drawFilledRect( x0, y0 + 1, x0 + 1, y1 - 1 );
	drawFilledRect( x1 - 1, y0 + 1, x1, y1 - 1 );
}

void XashSurface::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	vpoint_t topLeft, bottomRight, clippedTopLeft, clippedBottomRight;

	topLeft.point[0] = x0 + translate[0];
	topLeft.point[1] = y0 + translate[1];
	bottomRight.point[0] = x1 + translate[0];
	bottomRight.point[1] = y1 + translate[1];
	topLeft.coord[0] = topLeft.coord[1] = 0.0f;
	bottomRight.coord[0] = bottomRight.coord[1] = 1.0f;

	if( !g_scissor.clip( topLeft, bottomRight, clippedTopLeft, clippedBottomRight ))
		return;

	// the draw colour, not the text colour
	g_engine->SetupDrawingImage( drawColor );
	g_engine->DrawQuad( &clippedTopLeft, &clippedBottomRight );
}

void XashSurface::drawSetTextureRGBA( int id, const char *rgba, int wide, int tall )
{
	g_engine->UploadTexture( id, rgba, wide, tall );
}

void XashSurface::drawSetTexture( int id )
{
	g_engine->BindTexture( id );
}

void XashSurface::uploadFontPage( FontCache *cache, int index, const unsigned char *pixels )
{
	if( !cache->texture[index] )
		cache->texture[index] = g_engine->GenerateTexture();

	g_engine->UploadTexture( cache->texture[index], (const char *)pixels, FONT_PAGE_SIZE, FONT_PAGE_SIZE );
}

XashSurface::FontCache *XashSurface::buildFontCache( Font *font )
{
	const int pageBytes = FONT_PAGE_SIZE * FONT_PAGE_SIZE * 4;
	FontCache *cache = new FontCache;
	unsigned char *pixels = new unsigned char[pageBytes];
	int tall = font->getTall();
	int x = 0, y = 0, pageIndex = 0;
	bool pageLimitHit = false;

	memset( cache, 0, sizeof( *cache ));
	memset( pixels, 0, pageBytes );

	cache->id = font->getId();
	cache->generation = fontGeneration;

	// a1ba, TODO: use lightmap atlas generator here later
	for( int ch = 0; ch < 256; ch++ )
	{
		int a, b, c;

		// whitespace is never drawn and a wide space glyph would waste a lot of the page
		if( Q_isspace( ch ))
			continue;

		font->getCharABCwide( ch, a, b, c );

		if( x + b > FONT_PAGE_SIZE ) // doesn't fit in this row anymore
		{
			x = 0;
			y += tall + 1;

			if( y + tall > FONT_PAGE_SIZE ) // ...and the new row doesn't fit on this page
			{
				uploadFontPage( cache, pageIndex, pixels );

				if( ++pageIndex >= MAX_FONT_PAGES )
				{
					pageLimitHit = true;
					break;
				}

				memset( pixels, 0, pageBytes );
				x = y = 0;
			}
		}

		font->getCharRGBA( ch, x, y, FONT_PAGE_SIZE, FONT_PAGE_SIZE, pixels );

		cache->page[ch] = (unsigned char)pageIndex;
		cache->coord[ch][0] = (float)((double)x / FONT_PAGE_SIZE );
		cache->coord[ch][1] = (float)((double)y / FONT_PAGE_SIZE );
		cache->coord[ch][2] = (float)((double)( x + b ) / FONT_PAGE_SIZE );
		cache->coord[ch][3] = (float)((double)( y + tall ) / FONT_PAGE_SIZE );

		x += b + 1; // one pixel of padding
	}

	if( !pageLimitHit ) // the last, partially filled page
		uploadFontPage( cache, pageIndex++, pixels );

	cache->pageCount = pageIndex;

	delete[] pixels;

	// entries are appended, never updated in place: a rebuilt entry has to take precedence
	// over the one it replaces, and the lookup below is what makes that happen
	fontCacheDar.addElement( cache );

	return cache;
}

void XashSurface::drawSetTextFont( Font *font )
{
	currentFont = font;
	currentFontCache = nullptr;

	// a null font is a valid latch, subsequent text draws then do nothing
	if( !font )
		return;

	int id = font->getId();

	for( int i = 0; i < fontCacheDar.getCount(); i++ )
	{
		if( fontCacheDar[i]->id == id )
			currentFontCache = fontCacheDar[i]; // scan on, the last match wins
	}

	// an entry from a previous surface carries texture ids that no longer exist. In practice
	// this never fires, the constructor already cleared the cache
	if( currentFontCache && currentFontCache->generation != fontGeneration )
		currentFontCache = nullptr;

	if( !currentFontCache )
		currentFontCache = buildFontCache( font );
}

void XashSurface::drawPrintText( const char *text, int textLen )
{
	if( !text || textLen <= 0 )
		return;

	if( !currentFont || !currentFontCache )
		return;

	if( textColor[3] >= 255 )
		return;

	// FIXME: remove this, a remnant of original Xash3D feature with colorcodes in VGUI
	// horribly breaks mods
	if( textLen == 1 )
	{
		if( text[0] == '^' )
		{
			sawCaret = true;
			return;
		}

		if( sawCaret && text[0] >= '0' && text[0] <= '9' )
		{
			colorIndex = text[0] - '0';
			sawCaret = false;
			return;
		}

		sawCaret = false;
	}

	int color[4];

	if( colorIndex == DEFAULT_COLOR )
	{
		memcpy( color, textColor, sizeof( color ));
	}
	else
	{
		for( int i = 0; i < 3; i++ )
			color[i] = g_engine->GetColor( bound( 0, colorIndex, DEFAULT_COLOR ), i );

		color[3] = textColor[3]; // alpha always comes from the latched text colour
	}

	int x = textPos[0] + translate[0];
	int y = textPos[1] + translate[1];
	int tall = currentFont->getTall();
	int advance = 0;

	for( int i = 0; i < textLen; i++ )
	{
		int ch = g_engine->ProcessUtfChar( (unsigned char)text[i] );

		if( !ch ) // incomplete multi-byte sequence
			continue;

		if( ch < 0 || ch > 255 ) // the decoder shouldn't hand us these, but the tables are 256 entries
			continue;

		int a, b, c;

		currentFont->getCharABCwide( ch, a, b, c );
		advance += a;

		if( !Q_isspace( ch )) // no glyph was rasterised for whitespace
		{
			vpoint_t topLeft, bottomRight, clippedTopLeft, clippedBottomRight;

			// already translated, don't translate again
			topLeft.point[0] = x + advance;
			topLeft.point[1] = y;
			bottomRight.point[0] = topLeft.point[0] + b;
			bottomRight.point[1] = y + tall;
			topLeft.coord[0] = currentFontCache->coord[ch][0];
			topLeft.coord[1] = currentFontCache->coord[ch][1];
			bottomRight.coord[0] = currentFontCache->coord[ch][2];
			bottomRight.coord[1] = currentFontCache->coord[ch][3];

			if( g_scissor.clip( topLeft, bottomRight, clippedTopLeft, clippedBottomRight ))
			{
				g_engine->BindTexture( currentFontCache->texture[currentFontCache->page[ch]] );
				g_engine->SetupDrawingImage( color );
				g_engine->DrawQuad( &clippedTopLeft, &clippedBottomRight );
			}
		}

		advance += b + c;
	}

	// so that consecutive calls continue where this one stopped
	textPos[0] += advance;
}

void XashSurface::makeCurrent( const PaintState *ps )
{
	translate[0] = ps->translate[0];
	translate[1] = ps->translate[1];

	g_scissor.setRect( ps->clip[0], ps->clip[1], ps->clip[2], ps->clip[3] );
}

void XashSurface::pushMakeCurrent( Panel *panel, bool useInsets )
{
	if( paintStackPos >= MAX_PAINT_STACK )
	{
		vgui_dprintf( "vgui: paint state stack overflow, broken panel tree?\n" );
		return;
	}

	int inset[4] = { 0, 0, 0, 0 };
	int abs[4], clip[4];

	if( useInsets )
		panel->getInset( inset[0], inset[1], inset[2], inset[3] );

	panel->getAbsExtents( abs[0], abs[1], abs[2], abs[3] );
	panel->getClipRect( clip[0], clip[1], clip[2], clip[3] );

	PaintState *ps = &paintStack[paintStackPos++];

	ps->panel = panel;

	// every coordinate the panel draws at is relative to its inner upper-left corner...
	ps->translate[0] = abs[0] + inset[0];
	ps->translate[1] = abs[1] + inset[1];

	// ...but the clip stays the panel's full clip rect: borders deliberately draw into the inset ring
	memcpy( ps->clip, clip, sizeof( ps->clip ));

	makeCurrent( ps );
}

void XashSurface::popMakeCurrent( Panel *panel )
{
	if( paintStackPos <= 0 )
	{
		vgui_dprintf( "vgui: paint state stack underflow, broken panel tree?\n" );
		return;
	}

	if( paintStack[paintStackPos - 1].panel != panel )
	{
		vgui_dprintf( "vgui: paint state stack popped out of order, broken panel tree?\n" );
		return;
	}

	paintStackPos--;

	if( paintStackPos > 0 )
	{
		makeCurrent( &paintStack[paintStackPos - 1] );
	}
	else
	{
		// nothing draws outside a push/pop pair, but don't leave the last panel's state behind
		int abs[4];

		getPanel()->getAbsExtents( abs[0], abs[1], abs[2], abs[3] );

		translate[0] = translate[1] = 0;
		g_scissor.setRect( abs[0], abs[1], abs[2], abs[3] );
	}
}
