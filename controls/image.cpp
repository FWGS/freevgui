// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "image.h"

namespace vgui
{

ImagePanel::ImagePanel( Image *image )
{
	setImage( image );
}

void ImagePanel::setImage( Image *newImage )
{
	image = newImage;

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
	if( image )
	{
		drawSetColor( Scheme::SC_WHITE );
		image->doPaint( this );
	}
}

}
