// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_PROGRESSBAR_H
#define VGUI_PROGRESSBAR_H

#include "panel.h"

namespace vgui
{
class CLASSEXPORT ProgressBar : public Panel
{
public:
	ProgressBar( int );
protected:
	virtual void paintBackground() override;
public:
	virtual void setProgress( float );
	virtual int getSegmentCount();
private:
	int   _segmentCount;
	float _progress;
};
CHECK_STRUCT_SIZE( ProgressBar, 196, 272, 272 );

}

#endif // VGUI_PROGRESSBAR_H
