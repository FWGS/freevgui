// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <stdio.h>
#include "vgui_internal.h"
#include "surface.h"
#include "input.h"
#include "panel.h"
#include "app.h"
#include "font.h"
#include "controls/image.h"
#include "platform/common/font.h"

using namespace vgui;

SurfaceBase::SurfaceBase( Panel *p ) :
	pendingSwap( true ),
	rootPanel( p ),
	softwareCursor( new ImagePanel( nullptr )),
	cursor( nullptr )
{
	softwareCursor->setVisible( false );
	rootPanel->setSurfaceBaseTraverse( this );
	App::getInstance()->surfaceBaseCreated( this );
	softwareCursor->setParent( rootPanel );
}

Panel *SurfaceBase::getPanel()
{
	return rootPanel;
}

void SurfaceBase::requestSwap()
{
	pendingSwap = true;
}

void SurfaceBase::resetModeInfo()
{
	modes.removeAll();
}

int SurfaceBase::getModeInfoCount()
{
	return modes.getCount();
}

bool SurfaceBase::getModeInfo(int i, int &w, int &h, int &bpp)
{
	if( i >= 0 && i < modes.getCount() )
	{
		sscanf( modes[i], "%dx%dx%d", &w, &h, &bpp );
		return true;
	}
	return false;
}

SurfaceBase::~SurfaceBase()
{
	App::getInstance()->surfaceBaseDeleted( this );
}

void SurfaceBase::addModeInfo(int w, int h, int bpp)
{
	char buf[256];
	snprintf( buf, sizeof( buf ), "%dx%dx%d", w, h, bpp );

	modes.putElement( vgui_strdup( buf ));
}

App *SurfaceBase::getApp()
{
	return App::getInstance();
}

void SurfaceBase::setEmulatedCursorVisible( bool visible )
{
	softwareCursor->setVisible( visible );
}

void SurfaceBase::setEmulatedCursorPos( int x, int y )
{
	getPanel()->removeChild( softwareCursor );
	getPanel()->addChild( softwareCursor );
	getPanel()->screenToLocal( x, y );
	if( cursor && !softwareCursor->isVisible() )
	{
		int hotx, hoty;

		cursor->getHotspot( hotx, hoty );

		x -= hotx;
		y -= hoty;
	}

	softwareCursor->setPos( x, y );
}

// Ideally, Surface must be provided by the platform but I have no plans to revive Win32 backend
// even from archeological point of view it makes no sense because there are no standalone tools using VGUI1
// it's only used by one (!) game engine and we want free reimplementation of it, that's it
// 
// Additionally, Linux version of Surface never had real drawing and to me is a good candidate to ship it's stubs by default.
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
	int id, wide, tall;
};

static Texture *staticTextureCurrent = nullptr;
static Texture staticTexture[128];
static int staticTextureCount = 0;

static Texture *staticGetTextureById( int id )
{
	if( !staticTextureCurrent || id != staticTextureCurrent->id )
	{
		for( int i = 0; i < staticTextureCount; i++ )
		{
			if( staticTexture[i].id == id )
				return &staticTexture[i];
		}

		return nullptr;
	}

	return staticTextureCurrent;
}

Surface::Surface(Panel *p) :
	SurfaceBase( p ),
	impl( nullptr )
{
	createPlat();
	recreateContext();
}

void Surface::createPopup( Panel *p )
{
	rootPanel->setParent( nullptr );
	new Surface( rootPanel );
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
	if( impl->isFullscreen )
		return true;

	if( impl->fullscreenInfo[0] == w && impl->fullscreenInfo[1] == h && impl->fullscreenInfo[2] == bpp )
		return true;

	if( modes.getCount() == 0 )
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
	static int staticBindIndex = 0xa8c; // a1ba: magic number!
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
	if( f && f->impl ) f->impl->drawSetTextFont( impl );
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
		if( staticTextureCount >= (int)( sizeof( staticTexture ) / sizeof( staticTexture[0] )))
			return;

		tex = &staticTexture[staticTextureCount++];
		tex->id = id;
	}

	tex->wide = w;
	tex->tall = h;
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
	if( impl )
		return true;

	impl = new SurfacePlat;
	memset( impl, 0, sizeof( *impl ));

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

void Surface::setCursor( Cursor *newCursor )
{
	cursor = newCursor;

	if( cursor )
	{
		Bitmap *image = cursor->getBitmap();
		softwareCursor->setImage( image );

		cursor->getDefaultCursor();
	}
	else
		softwareCursor->setImage( nullptr );
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
