// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_SINGALS_H
#define VGUI_SINGALS_H

#include "vgui.h"
#include "input.h"

namespace vgui
{
class Frame;
class Panel;

class CLASSEXPORT ActionSignal {
public:
	virtual void actionPerformed( Panel * ) = 0;
};

class CLASSEXPORT ChangeSignal {
public:
	virtual void valueChanged( Panel * ) = 0;
};

class CLASSEXPORT FocusChangeSignal {
public:
	virtual void focusChanged( bool, Panel * ) = 0;
};

class CLASSEXPORT FrameSignal {
public:
	virtual void closing( Frame * ) = 0;
	virtual void minimizing( Frame *, bool ) = 0;
};

class CLASSEXPORT InputSignal {
public:
	virtual void cursorMoved( int, int, Panel * ) = 0;
	virtual void cursorEntered( Panel * ) = 0;
	virtual void cursorExited( Panel * ) = 0;
	virtual void mousePressed( MouseCode, Panel * ) = 0;
	virtual void mouseDoublePressed( MouseCode, Panel * ) = 0;
	virtual void mouseReleased( MouseCode, Panel * ) = 0;
	virtual void mouseWheeled( int, Panel * ) = 0;
	virtual void keyPressed( KeyCode, Panel * ) = 0;
	virtual void keyTyped( KeyCode, Panel * ) = 0;
	virtual void keyReleased( KeyCode, Panel * ) = 0;
	virtual void keyFocusTicked( Panel * ) = 0;
};

class CLASSEXPORT IntChangeSignal {
public:
	virtual void intChanged( int, Panel * ) = 0;
};

class RepaintSignal { // NO EXPORT BUT REFERENCED AS DAR<>
public:
	virtual void panelRepainted( Panel * ) = 0;
};

class CLASSEXPORT TickSignal {
public:
	virtual void ticked() = 0;
};
}

#endif // VGUI_SINGALS_H
