// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "surface.h"
#include "vgui_internal.h"
#include "input.h"
#include "panel.h"
#include "app.h"
#include "controls/image.h"
#include "font.h"
#include "platform/common/font.h"

using namespace vgui;

class vgui::SurfacePlat
{
public:
	int bitmapSize[2];
	int restoreInfo[4];
	bool isFullscreen;
	int fullscreenInfo[3];
};

class Texture
{
public:
	int _id;
	int _wide;
	int _tall;
	void *_dib;
};

static Texture *staticTextureCurrent = nullptr;
static Texture staticTexture[128];
static int staticTextureCount = 0;

// inlined in decompiled code, thanks we have inline data
static Texture *staticGetTextureById( int id )
{
	if( !staticTextureCurrent || id != staticTextureCurrent->_id )
	{
		for( int i = 0; i < staticTextureCount; i++ )
		{
			if( staticTexture[i]._id == id )
				return &staticTexture[i];
		}

		return nullptr;
	}

	return staticTextureCurrent;
}

int Surface::getModeInfoCount()
{
	resetModeInfo();
	addModeInfo( 640, 480, 16 );
	addModeInfo( 800, 600, 16 );

	return SurfaceBase::getModeInfoCount();
}

void Surface::setTitle( const char * )
{

}

bool Surface::setFullscreenMode( int w, int h, int bpp )
{
	if( _plat->isFullscreen )
		return true;

	int info[3];
	info[0] = w;
	info[1] = h;
	info[2] = bpp;

	if( _plat->fullscreenInfo[0] == info[0] && _plat->fullscreenInfo[1] == info[1] && _plat->fullscreenInfo[2] == info[2] )
		return true;

	if( _modeInfoDar.getCount() == 0 )
		getModeInfoCount();

	return false;
}

void Surface::setWindowedMode()
{
}

void Surface::setAsTopMost( bool )
{
}

bool Surface::hasFocus()
{
	return true;
}

bool Surface::isWithin( int, int )
{
	return true;
}

void Surface::GetMousePos( int &x, int &y )
{

}

int Surface::createNewTextureID()
{
	static int staticBindIndex = 0xa8c;
	return staticBindIndex++;
}

void Surface::drawSetColor( int, int, int, int )
{
}

void Surface::drawFilledRect( int, int, int, int )
{
}

void Surface::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	drawFilledRect( x0,      y0,     x1,     y0 + 1 );
	drawFilledRect( x0,      y1 - 1, x1,     y1     );
	drawFilledRect( x0,      y0 + 1, x0 + 1, y1 - 1 );
	drawFilledRect( x1 + -1, y0 + 1, x1,     y1 - 1 );
}

void Surface::drawSetTextFont( Font *f )
{
	if( !f )
		f->_plat->drawSetTextFont( _plat );
}

void Surface::drawSetTextColor( int, int, int, int )
{
}

void Surface::drawSetTextPos( int, int )
{
}

void Surface::drawPrintText( const char *, int )
{
}

void Surface::drawSetTextureRGBA( int id, const char *rgba, int w, int h )
{
	Texture *tex = staticGetTextureById( id );

	if( !tex )
	{
		if( staticTextureCount >= sizeof( staticTexture ))
			return;

		tex = &staticTexture[staticTextureCount++];
		tex->_id = id;
	}
}

void Surface::drawSetTexture( int id )
{
	staticTextureCurrent = staticGetTextureById( id );
}

void Surface::drawTexturedRect( int, int, int, int )
{
}

void Surface::invalidate( Panel * )
{
}

bool Surface::createPlat()
{
	// a1ba: shouldn't we allocate SurfacePlat here????
	return true;
}

bool Surface::recreateContext()
{
	int w, h;
	getPanel()->getSize( w, h );

	return true;
}

void Surface::enableMouseCapture( bool )
{
}

void Surface::setCursor( Cursor *cursor )
{
	_currentCursor = cursor;

	if( cursor )
	{
		Bitmap *image = cursor->getBitmap();
		_emulatedCursor->setImage( image );

		cursor->getDefaultCursor();
	}
	else
		_emulatedCursor->setImage( nullptr );
}

void Surface::swapBuffers()
{
}

void Surface::pushMakeCurrent( Panel *panel, bool useInsets )
{
	int inset[4], absThis[4], absPanel[4], clipRect[4];

	panel->getInset( inset[0], inset[1], inset[2], inset[3] );

	if( !useInsets )
	{
		inset[0] = 0;
		inset[1] = 0;
		inset[2] = 0;
		inset[3] = 0;
	}

	getPanel()->getAbsExtents( absThis[0], absThis[1], absThis[2], absThis[3] );
	panel->getAbsExtents( absPanel[0], absPanel[1], absPanel[2], absPanel[3] );
	panel->getClipRect( clipRect[0], clipRect[1], clipRect[2], clipRect[3] );
}

void Surface::popMakeCurrent( Panel *panel )
{
}

void Surface::applyChanges()
{
}
