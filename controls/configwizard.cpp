// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/configwizard.h"

using namespace vgui;

ConfigWizard::ConfigWizard( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void ConfigWizard::setSize( int wide, int tall )
{
}

Panel *ConfigWizard::getClient()
{
	return NULL;
}

TreeFolder *ConfigWizard::getFolder()
{
	return NULL;
}
