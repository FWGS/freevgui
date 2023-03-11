// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

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
CHECK_STRUCT_SIZE( ButtonController, 4 );

class CLASSEXPORT ButtonGroup
{
public:
	virtual void addButton( Button *b );
	virtual void setSelected( Button *b );

protected:
	Dar<Button*> _buttonDar;
};
CHECK_STRUCT_SIZE( ButtonGroup, 16 );

class CLASSEXPORT Button : public Label
{
public:
	Button( const char *text, int x, int y, int w, int h );
	Button( const char *text, int x, int y );
	virtual void setSelected( bool select );
	virtual void setSelectedDirect( bool select );
	virtual void setArmed( bool armed );
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
	void init();
protected:
	virtual void setButtonController( ButtonController *bc );
	virtual void paintBackground() override;

	char* _text;
	bool  _armed, _selected, _buttonBorderEnabled;
	Dar<ActionSignal*> _actionSignalDar;
	int                _mouseClickMask;
	ButtonGroup*       _buttonGroup;
	ButtonController*  _buttonController;
};
CHECK_STRUCT_SIZE( Button, 240 );

class CLASSEXPORT ToggleButton : public Button
{
public:
	ToggleButton( const char*, int, int, int, int );
	ToggleButton( const char*, int, int );
};
CHECK_STRUCT_SIZE( ToggleButton, sizeof( Button ));

class CLASSEXPORT CheckButton : public ToggleButton
{
public:
	CheckButton( const char*, int, int, int, int );
	CheckButton( const char*, int, int );
protected:
	virtual void paintBackground() override;
};
CHECK_STRUCT_SIZE( CheckButton, sizeof( ToggleButton ));

class CLASSEXPORT RadioButton : public ToggleButton
{
public:
	RadioButton( const char*, int, int, int, int );
	RadioButton( const char*, int, int );
protected:
	virtual void paintBackground() override;
};
CHECK_STRUCT_SIZE( RadioButton, sizeof( ToggleButton ));

}

#endif // VGUI_BUTTON_H
