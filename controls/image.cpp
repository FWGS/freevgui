// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "image.h"

namespace vgui
{

ImagePanel::ImagePanel( Image *image )
{
	setImage( image );
}

void ImagePanel::setImage( Image *image )
{
	_image = image;

	if( image )
	{
		int w, h;

		image->getSize( w, h );
		setSize( w, h );
	}

	repaint();
}

void ImagePanel::paintBackground()
{
	if( _image )
	{
		drawSetColor( Scheme::SC_WHITE );
		_image->doPaint( this );
	}
}

}
