// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_CONFIGWIZARD_H
#define VGUI_CONFIGWIZARD_H

#include "panel.h"
#include "controls/button.h"
#include "controls/treefolder.h"

namespace vgui
{
class CLASSEXPORT ConfigWizard : public Panel
{
public:
	ConfigWizard( int, int, int, int );
	virtual void setSize( int, int ) override;
	virtual Panel* getClient();
	virtual TreeFolder* getFolder();
protected:
	TreeFolder *treeFolder;
	Panel *client;
	Button *okButton, *cancelButton, *applyButton, *helpButton;
};
CHECK_STRUCT_SIZE( ConfigWizard, 212, 312, 312 );
}

#endif // VGUI_CONFIGWIZARD_H
