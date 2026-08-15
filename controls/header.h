// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_HEADERPANEL_H
#define VGUI_HEADERPANEL_H

#include "panel.h"
#include "signals.h"

namespace vgui
{
class CLASSEXPORT HeaderPanel : public Panel
{
protected:
	virtual void performLayout();

public:
	HeaderPanel( int x, int y, int w, int h );
	virtual void addSectionPanel( Panel *p );
	virtual void setSliderPos( int i, int pos );
	virtual int  getSectionCount();
	virtual void getSectionExtents( int i, int &x_left, int &x_right );
	virtual void addChangeSignal( ChangeSignal *s );
	virtual void fireChangeSignal();
	virtual void privateCursorMoved( int x, int y, Panel *p );
	virtual void privateMousePressed( MouseCode code, Panel *p );
	virtual void privateMouseReleased( MouseCode code, Panel *p );

private:
	Dar<Panel*> _sliderPanelDar, _sectionPanelDar;
	Dar<ChangeSignal*> _changeSignalDar;
	Panel* _sectionLayer;
	int  _sliderWide;
	bool _dragging;
	int  _dragSliderIndex;
	int  _dragSliderStartPos;
	int  _dragSliderStartX;
};
CHECK_STRUCT_SIZE( HeaderPanel, 248 );

};

#endif // VGUI_HEADERPANEL_H

