// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/tab.h"

using namespace vgui;

TabPanel::TabPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

Panel *TabPanel::addTab( const char *name )
{
	return NULL;
}

void TabPanel::setSelectedTab( Panel *panel )
{
}

void TabPanel::setSize( int wide, int tall )
{
}

void TabPanel::recomputeLayoutTop()
{
}

void TabPanel::recomputeLayout()
{
}
