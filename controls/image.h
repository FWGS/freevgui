// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_IMAGEPANEL_H
#define VGUI_IMAGEPANEL_H

#include "panel.h"

namespace vgui
{

class CLASSEXPORT ImagePanel : public Panel {
public:
	ImagePanel() : _image( NULL ) { }
	ImagePanel( Image *image );
	virtual void setImage( Image *image );
protected:
	virtual void paintBackground() override;
	Image* _image;
};

CHECK_STRUCT_SIZE( ImagePanel, 192 );

}

#endif // VGUI_IMAGEPANEL_H
