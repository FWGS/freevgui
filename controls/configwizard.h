// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

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
	TreeFolder *_treeFolder;
	Panel *_client;
	Button *_okButton, *_cancelButton, *_applyButton, *_helpButton;
};
CHECK_STRUCT_SIZE( ConfigWizard, 212 );
}

#endif // VGUI_CONFIGWIZARD_H
