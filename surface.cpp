// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include <stdio.h>
#include "vgui_internal.h"
#include "surface.h"
#include "input.h"
#include "panel.h"
#include "app.h"
#include "controls/image.h"

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
	Q_snprintf( buf, sizeof( buf ), "%dx%dx%d", w, h, bpp );

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

Surface::Surface(Panel *p) :
	SurfaceBase( p )
{
	createPlat();
	recreateContext();
}

void Surface::createPopup( Panel *p )
{
	_embeddedPanel->setParent( nullptr );
	new Surface( _embeddedPanel );
}
