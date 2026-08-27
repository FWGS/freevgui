// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

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

	bool opened;
public:
	virtual void setOpenedTraverse( bool open );
	virtual void setOpened( bool open );
	virtual bool isOpened();
};

#if defined( _MSC_VER )
// TODO: validate the sizes!!!
CHECK_STRUCT_SIZE( TreeFolder, 192, 272, 272 );
#else
CHECK_STRUCT_SIZE( TreeFolder, 188, 264, 264 );
#endif

}

#endif // VGUI_TREEFOLDER_H
