// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_TABPANEL_H
#define VGUI_TABPANEL_H

#include "controls/button.h"

namespace vgui
{
class CLASSEXPORT TabPanel : public Panel
{
public:
	TabPanel( int, int, int, int );

	enum TabPlacement : int32_t
	{
		TOP = 0, BOTTOM, LEFT, RIGHT
	};

	virtual Panel* addTab( const char* );
	virtual void setSelectedTab( Panel* );
	virtual void setSize( int, int ) override;
protected:
	virtual void recomputeLayoutTop();
	virtual void recomputeLayout();
private:
	TabPlacement _tabPlacement; // doesn't work in vgui
	Panel *_tabArea;
	Panel *_clientArea;
	Panel *_selectedTab;
	Panel *_selectedPanel;
	ButtonGroup *_buttonGroup;
};
CHECK_STRUCT_SIZE( TabPanel, 212 );
}

#endif // VGUI_TABPANEL_H
