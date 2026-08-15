// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_LISTPANEL_H
#define VGUI_LISTPANEL_H

#include "panel.h"
#include "scroll.h"

namespace vgui
{
class CLASSEXPORT ListPanel : public Panel
{
public:
	ListPanel( int, int, int, int );
	virtual void setSize( int, int ) override;
	virtual void addString( const char* );
	virtual void addItem( Panel* );
	virtual void setPixelScroll( int );
	virtual void translatePixelScroll( int );
protected:
	virtual void performLayout() override;
	virtual void paintBackground() override;

	Panel* _vpanel;
	ScrollBar* _scroll;
};
CHECK_STRUCT_SIZE( ListPanel, 196 );
}

#endif // VGUI_LISTPANEL_H
