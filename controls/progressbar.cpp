// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/progressbar.h"

using namespace vgui;

ProgressBar::ProgressBar( int segmentCount ) : Panel( 0, 0, 10, 110 ),
	segmentCount( segmentCount ), segmentsLit( 0.0f )
{
}

void ProgressBar::paintBackground()
{
	int wide, tall;

	getPaintSize( wide, tall );

	drawSetColor( Scheme::SC_SECONDARY2 );
	drawFilledRect( 0, 0, wide, tall );

	int segmentGap = 2;
	int segmentWide = wide / segmentCount - segmentGap;
	int litSeg = (int)segmentsLit;
	int x = 0;

	for( int i = 0; i < litSeg; i++ )
	{
		drawSetColor( 0, 0, 100, 0 );
		drawFilledRect( x, 0, x + segmentWide, tall );
		x += segmentWide + segmentGap;
	}

	if( segmentCount > segmentsLit )
	{
		float frac = segmentsLit - (int)segmentsLit;

		drawSetColor( 0, 0, 255 - frac * 155, 0 );
		drawFilledRect( x, 0, x + segmentWide, tall );
	}
}

void ProgressBar::setProgress( float progress )
{
	if( progress != segmentsLit )
	{
		segmentsLit = progress;
		repaint();
	}
}

int ProgressBar::getSegmentCount()
{
	return segmentCount;
}
