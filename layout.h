// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_LAYOUT_H
#define VGUI_LAYOUT_H

#include "vgui.h"

namespace vgui
{
class Panel;

class CLASSEXPORT Layout
{
public:
	virtual void performLayout( Panel * );
};
CHECK_STRUCT_SIZE( Layout, 4 );

class CLASSEXPORT LayoutInfo
{
public:
	virtual LayoutInfo *getThis() = 0;
};
CHECK_STRUCT_SIZE( LayoutInfo, 4 );

class CLASSEXPORT BorderLayout : public Layout
{
public:
	enum Alignment : int32_t { CENTER = 0, TOP, BOTTOM, RIGHT, LEFT };
	BorderLayout( int inset );

	virtual void performLayout( Panel * ) override;
	virtual LayoutInfo *createLayoutInfo( Alignment );
private:
	int _inset;
};
CHECK_STRUCT_SIZE( BorderLayout, 8 );

class CLASSEXPORT FlowLayout : public Layout
{
public:
	FlowLayout( int hgap );

	virtual void performLayout( Panel * ) override;
private:
	int _hgap;
};
CHECK_STRUCT_SIZE( FlowLayout, 8 );

class CLASSEXPORT StackLayout : public Layout
{
public:
	StackLayout( int vgap, bool fitWide );

	virtual void performLayout( Panel * ) override;
private:
	int _vgap;
	bool _fitWide;
};
CHECK_STRUCT_SIZE( StackLayout, 12 );
}

#endif // VGUI_LAYOUT_H

