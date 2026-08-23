// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_LAYOUT_H
#define VGUI_LAYOUT_H

#include "vgui.h"

namespace vgui
{
class Panel;

class CLASSEXPORT Layout
{
public:
	Layout();
	virtual void performLayout( Panel * );
};
CHECK_STRUCT_SIZE( Layout, 4, 8, 8 );

class CLASSEXPORT LayoutInfo
{
public:
	virtual LayoutInfo *getThis() = 0;
};
CHECK_STRUCT_SIZE( LayoutInfo, 4, 8, 8 );

class CLASSEXPORT BorderLayout : public Layout
{
public:
	enum Alignment : int32_t { CENTER = 0, TOP, BOTTOM, RIGHT, LEFT };
	BorderLayout( int inset );

	virtual void performLayout( Panel * ) override;
	virtual LayoutInfo *createLayoutInfo( Alignment );
private:
	int inset;
};
CHECK_STRUCT_SIZE( BorderLayout, 8, 16, 16 );

class CLASSEXPORT FlowLayout : public Layout
{
public:
	FlowLayout( int hgap );

	virtual void performLayout( Panel * ) override;
private:
	int horizontalGap;
};
CHECK_STRUCT_SIZE( FlowLayout, 8, 16, 16 );

class CLASSEXPORT StackLayout : public Layout
{
public:
	StackLayout( int vgap, bool fitWide );

	virtual void performLayout( Panel * ) override;
private:
	int verticalGap;
	bool fitWide;
};
CHECK_STRUCT_SIZE( StackLayout, 12, 16, 16 );
}

#endif // VGUI_LAYOUT_H

