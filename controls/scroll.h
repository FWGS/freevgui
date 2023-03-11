// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_SCROLLBAR_H
#define VGUI_SCROLLBAR_H

#include "panel.h"
#include "signals.h"
#include "button.h"

namespace vgui
{
class CLASSEXPORT Slider : public Panel
{
public:
	Slider( int, int, int, int, bool );
	virtual void setValue( int value );
	virtual int getValue();
	virtual bool isVertical();
	virtual void addIntChangeSignal( IntChangeSignal *s );
	virtual void setRange( int imin, int imax );
	virtual void getRange( int &imin, int &imax );
	virtual void setRangeWindow( int rangeWindow );
	virtual void setRangeWindowEnabled( bool enable );
	virtual void setSize( int w, int h ) override;
	virtual void getNobPos( int &imin, int &imax );
	virtual bool hasFullRange();
	virtual void setButtonOffset( int off );

private:
	virtual void recomputeNobPosFromValue();
	virtual void recomputeValueFromNobPos();

	bool _vertical, _dragging;
	int _nobPos[2], _nobDragStartPos[2], _dragStartPos[2];
	Dar<IntChangeSignal*> _intChangeSignalDar;
	int _range[2], _value, _rangeWindow;
	bool _rangeWindowEnabled;
	int _buttonOffset;
public:
	virtual void privateCursorMoved( int, int, Panel* );
	virtual void privateMousePressed( MouseCode, Panel* );
	virtual void privateMouseReleased( MouseCode, Panel* );
protected:
	virtual void fireIntChangeSignal();
	virtual void paintBackground() override;

};
CHECK_STRUCT_SIZE( Slider, 252 );

class CLASSEXPORT ScrollBar : public Panel
{
public:
	ScrollBar( int, int, int, int, bool );
	virtual void setValue( int );
	virtual int getValue();
	virtual void addIntChangeSignal( IntChangeSignal* );
	virtual void setRange( int, int );
	virtual void setRangeWindow( int );
	virtual void setRangeWindowEnabled( bool );
	virtual void setSize( int, int ) override;
	virtual bool isVertical();
	virtual bool hasFullRange();
	virtual void setButton( Button*, int );
	virtual Button* getButton( int );
	virtual void setSlider( Slider* );
	virtual Slider* getSlider();
	virtual void doButtonPressed( int );
	virtual void setButtonPressedScrollValue( int );
	virtual void validate();
	virtual void fireIntChangeSignal();
protected:
	virtual void performLayout() override;

	Button* _button[2];
	Slider* _slider;
	Dar<IntChangeSignal*> _intChangeSignalDar;
	int _buttonPressedScrollValue;
};
CHECK_STRUCT_SIZE( ScrollBar, 216 );

class CLASSEXPORT ScrollPanel : public Panel
{
protected:
	virtual void setSize( int, int ) override;
public:
	ScrollPanel( int, int, int, int );

	virtual void setScrollBarVisible( bool, bool );
	virtual void setScrollBarAutoVisible( bool, bool );
	virtual Panel* getClient();
	virtual Panel* getClientClip();
	virtual void setScrollValue( int, int );
	virtual void getScrollValue( int&, int& );
	virtual void recomputeClientSize();
	virtual ScrollBar* getHorizontalScrollBar();
	virtual ScrollBar* getVerticalScrollBar();
	virtual void validate();
	virtual void recomputeScroll();
private:
	Panel* _clientClip;
	Panel* _client;
	ScrollBar* _horizontalScrollBar;
	ScrollBar* _verticalScrollBar;
	bool _autoVisible[2];
};
CHECK_STRUCT_SIZE( ScrollPanel, 208 );
}

#endif // VGUI_SCROLLBAR_H
