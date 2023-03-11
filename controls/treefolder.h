// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_TREEFOLDER_H
#define VGUI_TREEFOLDER_H

#include "panel.h"

namespace vgui
{
class CLASSEXPORT TreeFolder : public Panel
{
public:
	TreeFolder( const char *name );
	TreeFolder( const char *name, int x, int y );
protected:
	virtual void paintBackground() override;
	virtual void init( const char* );

	bool _opened;
public:
	virtual void setOpenedTraverse( bool open );
	virtual void setOpened( bool open );
	virtual bool isOpened();
};
CHECK_STRUCT_SIZE( TreeFolder, 188 );
}

#endif // VGUI_TREEFOLDER_H
