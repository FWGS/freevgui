// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_MESSAGEBOX_H
#define VGUI_MESSAGEBOX_H

#include "controls/frame.h"
#include "controls/button.h"

namespace vgui
{
class CLASSEXPORT MessageBox : public Frame
{
public:
	MessageBox( const char*, const char*, int, int );
protected:
	virtual void performLayout() override;

	Label*  messageLabel;
	Button* okButton;
	Dar<ActionSignal*> actionSignals;
public:
	virtual void addActionSignal( ActionSignal* );
	virtual void fireActionSignal();
};
CHECK_STRUCT_SIZE( MessageBox, 292, 456, 456 );
}

#endif // VGUI_MESSAGEBOX_H
