// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

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

	Label*  _messageLabel;
	Button* _okButton;
	Dar<ActionSignal*> _actionSignalDar;
public:
	virtual void addActionSignal( ActionSignal* );
	virtual void fireActionSignal();
};
CHECK_STRUCT_SIZE( MessageBox, 292 );
}

#endif // VGUI_MESSAGEBOX_H
