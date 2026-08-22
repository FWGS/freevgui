// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

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
	virtual void setValue( int newValue );
	virtual int getValue();
	virtual bool isVertical();
	virtual void addIntChangeSignal( IntChangeSignal *s );
	virtual void setRange( int imin, int imax );
	virtual void getRange( int &imin, int &imax );
	virtual void setRangeWindow( int newRangeWindow );
	virtual void setRangeWindowEnabled( bool enable );
	virtual void setSize( int w, int h ) override;
	virtual void getNobPos( int &imin, int &imax );
	virtual bool hasFullRange();
	virtual void setButtonOffset( int off );

private:
	virtual void recomputeNobPosFromValue();
	virtual void recomputeValueFromNobPos();

	bool vertical, dragging;
	int knobPos[2], knobDragStartPos[2], dragStartPos[2];
	Dar<IntChangeSignal*> intChangeSignals;
	int range[2], value, rangeWindow;
	bool rangeWindowEnabled;
	int buttonOffset;
public:
	virtual void privateCursorMoved( int, int, Panel* );
	virtual void privateMousePressed( MouseCode, Panel* );
	virtual void privateMouseReleased( MouseCode, Panel* );
protected:
	virtual void fireIntChangeSignal();
	virtual void paintBackground() override;

};
CHECK_STRUCT_SIZE( Slider, 252, 328, 328 );

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

	Button* buttons[2];
	Slider* slider;
	Dar<IntChangeSignal*> intChangeSignals;
	int buttonPressedScrollValue;
};
CHECK_STRUCT_SIZE( ScrollBar, 216, 312, 312 );

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
	Panel* clientClip;
	Panel* client;
	ScrollBar* horizontalScrollBar;
	ScrollBar* verticalScrollBar;
	bool autoVisible[2];
};
CHECK_STRUCT_SIZE( ScrollPanel, 208, 304, 304 );
}

#endif // VGUI_SCROLLBAR_H
