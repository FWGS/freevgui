// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "vgui_internal.h"
#include "support.h"

using namespace vgui;

Scissor vgui::g_scissor;

Scissor::Scissor() : enabled( false )
{
	rect[0] = rect[1] = rect[2] = rect[3] = 0;
}

void Scissor::enable()
{
	enabled = true;
}

void Scissor::disable()
{
	enabled = false;
}

void Scissor::setRect( int x0, int y0, int x1, int y1 )
{
	rect[0] = x0;
	rect[1] = y0;

	// an inverted rectangle means the panel tree handed us a broken clip rect. Collapse it
	// to an empty one so nothing draws, instead of letting the intersection below pass
	rect[2] = Q_max( x0, x1 );
	rect[3] = Q_max( y0, y1 );
}

bool Scissor::clip( const vpoint_t &topLeft, const vpoint_t &bottomRight, vpoint_t &clippedTopLeft, vpoint_t &clippedBottomRight ) const
{
	if( !enabled )
	{
		clippedTopLeft = topLeft;
		clippedBottomRight = bottomRight;
		return true;
	}

	float point[2][2];

	point[0][0] = Q_max( topLeft.point[0], (float)rect[0] );
	point[0][1] = Q_max( topLeft.point[1], (float)rect[1] );
	point[1][0] = Q_min( bottomRight.point[0], (float)rect[2] );
	point[1][1] = Q_min( bottomRight.point[1], (float)rect[3] );

	// a degenerate rectangle (zero width or height) is still submitted, only an inverted one is dropped
	if( point[0][0] > point[1][0] || point[0][1] > point[1][1] )
		return false;

	float coord[2][2];

	for( int axis = 0; axis < 2; axis++ )
	{
		float span = bottomRight.point[axis] - topLeft.point[axis];

		for( int corner = 0; corner < 2; corner++ )
		{
			// how far along the input rectangle the clipped edge ended up. A rectangle
			// degenerate on this axis has no fraction to speak of, take the middle
			float frac = span != 0.0f ? ( point[corner][axis] - topLeft.point[axis] ) / span : 0.5f;

			coord[corner][axis] = topLeft.coord[axis] + ( bottomRight.coord[axis] - topLeft.coord[axis] ) * frac;
		}
	}

	clippedTopLeft.point[0] = point[0][0];
	clippedTopLeft.point[1] = point[0][1];
	clippedTopLeft.coord[0] = coord[0][0];
	clippedTopLeft.coord[1] = coord[0][1];

	clippedBottomRight.point[0] = point[1][0];
	clippedBottomRight.point[1] = point[1][1];
	clippedBottomRight.coord[0] = coord[1][0];
	clippedBottomRight.coord[1] = coord[1][1];

	return true;
}
