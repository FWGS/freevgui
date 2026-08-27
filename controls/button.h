// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_BUTTON_H
#define VGUI_BUTTON_H

#include "controls/label.h"

namespace vgui
{
class Button;

class CLASSEXPORT ButtonController
{
public:
	virtual void addSignals( Button* ) = 0;
	virtual void removeSignals( Button* ) = 0;
};
CHECK_STRUCT_SIZE( ButtonController, 4, 8, 8 );

class CLASSEXPORT ButtonGroup
{
public:
	virtual void addButton( Button *b );
	virtual void setSelected( Button *b );

protected:
	Dar<Button*> buttons;
};
CHECK_STRUCT_SIZE( ButtonGroup, 16, 24, 24 );

class CLASSEXPORT Button : public Label
{
public:
	Button( const char *text, int x, int y, int w, int h );
	Button( const char *text, int x, int y );
	virtual void setSelected( bool select );
	virtual void setSelectedDirect( bool select );
	virtual void setArmed( bool state );
	virtual bool isSelected();
	virtual void doClick();
	virtual void addActionSignal( ActionSignal *s );
	virtual void setButtonGroup( ButtonGroup *bg );
	virtual bool isArmed();
	virtual void setButtonBorderEnabled( bool enable );
	virtual void setMouseClickEnabled( MouseCode code, bool enable );
	virtual bool isMouseClickEnabled( MouseCode code );
	virtual void fireActionSignal();
	virtual Panel* createPropertyPanel() override;

private:
	void init();

protected:
	virtual void setButtonController( ButtonController *bc );
	virtual void paintBackground() override;

	char* text;
	bool  armed, selected, buttonBorderEnabled;
	Dar<ActionSignal*> actionSignals;
	int                mouseClickMask;
	ButtonGroup*       buttonGroup;
	ButtonController*  buttonController;
};
CHECK_STRUCT_SIZE( Button, 240, 344, 344 );

class CLASSEXPORT ToggleButton : public Button
{
public:
	ToggleButton( const char*, int, int, int, int );
	ToggleButton( const char*, int, int );
};
CHECK_STRUCT_SIZE_EQ( ToggleButton, Button );

class CLASSEXPORT CheckButton : public ToggleButton
{
public:
	CheckButton( const char*, int, int, int, int );
	CheckButton( const char*, int, int );
protected:
	virtual void paintBackground() override;
};
CHECK_STRUCT_SIZE_EQ( CheckButton, ToggleButton );

class CLASSEXPORT RadioButton : public ToggleButton
{
public:
	RadioButton( const char*, int, int, int, int );
	RadioButton( const char*, int, int );
protected:
	virtual void paintBackground() override;
};
CHECK_STRUCT_SIZE_EQ( RadioButton, ToggleButton );

}

#endif // VGUI_BUTTON_H
