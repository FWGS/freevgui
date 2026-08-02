// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/progressbar.h"

using namespace vgui;

ProgressBar::ProgressBar( int segmentCount ) : Panel( 0, 0, 10, 110 ),
	_segmentCount( segmentCount ), _progress( 0.0f )
{
}

void ProgressBar::paintBackground()
{
	int wide, tall;

	getPaintSize( wide, tall );

	drawSetColor( Scheme::SC_SECONDARY2 );
	drawFilledRect( 0, 0, wide, tall );

	int segmentGap = 2;
	int segmentWide = wide / _segmentCount - segmentGap;
	int litSeg = (int)_progress;
	int x = 0;

	for( int i = 0; i < litSeg; i++ )
	{
		drawSetColor( 0, 0, 100, 0 );
		drawFilledRect( x, 0, x + segmentWide, tall );
		x += segmentWide + segmentGap;
	}

	if( _segmentCount > _progress )
	{
		float frac = _progress - (int)_progress;

		drawSetColor( 0, 0, 255 - frac * 155, 0 );
		drawFilledRect( x, 0, x + segmentWide, tall );
	}
}

void ProgressBar::setProgress( float progress )
{
	if( progress != _progress )
	{
		_progress = progress;
		repaint();
	}
}

int ProgressBar::getSegmentCount()
{
	return _segmentCount;
}
