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
	_needsSwap( true ),
	_embeddedPanel( p ),
	_emulatedCursor( new ImagePanel( nullptr )),
	_currentCursor( nullptr )
{
	_emulatedCursor->setVisible( false );
	_embeddedPanel->setSurfaceBaseTraverse( this );
	App::getInstance()->surfaceBaseCreated( this );
	_emulatedCursor->setParent( _embeddedPanel );
}

Panel *SurfaceBase::getPanel()
{
	return _embeddedPanel;
}

void SurfaceBase::requestSwap()
{
	_needsSwap = true;
}

void SurfaceBase::resetModeInfo()
{
	_modeInfoDar.removeAll();
}

int SurfaceBase::getModeInfoCount()
{
	return _modeInfoDar.getCount();
}

bool SurfaceBase::getModeInfo(int i, int &w, int &h, int &bpp)
{
	if( i >= 0 && i < _modeInfoDar.getCount() )
	{
		sscanf( _modeInfoDar[i], "%dx%dx%d", &w, &h, &bpp );
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

	_modeInfoDar.putElement( vgui_strdup( buf ));
}

App *SurfaceBase::getApp()
{
	return App::getInstance();
}

void SurfaceBase::setEmulatedCursorVisible( bool visible )
{
	_emulatedCursor->setVisible( visible );
}

void SurfaceBase::setEmulatedCursorPos( int x, int y )
{
	getPanel()->removeChild( _emulatedCursor );
	getPanel()->addChild( _emulatedCursor );
	getPanel()->screenToLocal( x, y );
	if( _currentCursor && !_emulatedCursor->isVisible() )
	{
		int hotx, hoty;

		_currentCursor->getHotspot( hotx, hoty );

		x -= hotx;
		y -= hoty;
	}

	_emulatedCursor->setPos( x, y );
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
	int _id, _wide, _tall;
	void *_dib;
};

static Texture *staticTextureCurrent = nullptr;
static Texture staticTexture[128];
static int staticTextureCount = 0;

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

Surface::Surface(Panel *p) :
	SurfaceBase( p ),
	_plat( nullptr )
{
	createPlat();
	recreateContext();
}

void Surface::createPopup( Panel *p )
{
	_embeddedPanel->setParent( nullptr );
	new Surface( _embeddedPanel );
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

	if( _plat->fullscreenInfo[0] == w && _plat->fullscreenInfo[1] == h && _plat->fullscreenInfo[2] == bpp )
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
	if( f && f->_plat ) f->_plat->drawSetTextFont( _plat );
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
		tex->_id = id;
	}

	tex->_wide = w;
	tex->_tall = h;
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
	if( _plat )
		return true;

	_plat = new SurfacePlat;
	memset( _plat, 0, sizeof( *_plat ));

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
