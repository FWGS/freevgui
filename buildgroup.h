// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_BUILDGROUP_H
#define VGUI_BUILDGROUP_H

#include "vgui.h"
#include "input.h"

namespace vgui
{
class ChangeSignal;
class Cursor;
class Panel;

class CLASSEXPORT BuildGroup
{
	friend class Panel;
public:
	BuildGroup();
	virtual void setEnabled( bool enable );
	virtual bool isEnabled();
	virtual void addCurrentPanelChangeSignal( ChangeSignal *signal );
	virtual Panel *getCurrentPanel();
	virtual void copyPropertiesToClipboard();

private:
	virtual void applySnap( Panel *p );
	virtual void fireCurrentPanelChangeSignal();

	bool _enabled;
	int _snapX, _snapY;
	Cursor *_cursor_sizenwse;
	Cursor *_cursor_sizenesw;
	Cursor *_cursor_sizewe;
	Cursor *_cursor_sizens;
	Cursor *_cursor_sizeall;

	bool _dragging;
	MouseCode _dragMouseCode;
	int _dragStartPanelPos[2];
	int _dragStartCursorPos[2];

	Panel *_currentPanel;
	Dar<ChangeSignal *> _currentPanelChangeSignalDar;

	Dar<Panel *> _panelDar;
	Dar<char *> _panelNameDar;
protected:
	virtual void panelAdded( Panel *p, const char *str );
	virtual void cursorMoved( int x, int y, Panel *p );
	virtual void mousePressed( MouseCode, Panel * );
	virtual void mouseReleased( MouseCode, Panel * );
	virtual void mouseDoublePressed( MouseCode, Panel * );
	virtual void keyTyped( KeyCode, Panel * );
	virtual Cursor *getCursor( Panel * );
};

CHECK_STRUCT_SIZE( BuildGroup, 100, 144, 144 );
}

#endif // VGUI_BUILDGROUP_H
