// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_FRAME_H
#define VGUI_FRAME_H

#include "panel.h"
#include "controls/button.h"

namespace vgui
{
class CLASSEXPORT Frame : public Panel
{
public:
	Frame( int, int, int, int );

	virtual void setSize( int, int ) override;
	virtual void setInternal( bool );
	virtual void paintBackground();
	virtual bool isInternal();
	virtual Panel* getClient();
	virtual void setTitle( const char* );
	virtual void getTitle( char*, int );
	virtual void setMoveable( bool );
	virtual void setSizeable( bool );
	virtual bool isMoveable();
	virtual bool isSizeable();
	virtual void addFrameSignal( FrameSignal* );
	virtual void setVisible( bool ) override;
	virtual void setMenuButtonVisible( bool );
	virtual void setTrayButtonVisible( bool );
	virtual void setMinimizeButtonVisible( bool );
	virtual void setMaximizeButtonVisible( bool );
	virtual void setCloseButtonVisible( bool );
	virtual void fireClosingSignal();
	virtual void fireMinimizingSignal();
protected:
	char* title;
	bool internal, sizeable, moveable;
	Panel* topGrip;
	Panel* bottomGrip;
	Panel* leftGrip;
	Panel* rightGrip;
	Panel* topLeftGrip;
	Panel* topRightGrip;
	Panel* bottomLeftGrip;
	Panel* bottomRightGrip;
	Panel* captionGrip;
	Panel* client;
	Button* trayButton;
	Button* minimizeButton;
	Button* maximizeButton;
	Button* closeButton;
	Button* menuButton;
	Dar<FrameSignal*> frameSignals;
	Frame* resizeable;
};
CHECK_STRUCT_SIZE( Frame, 272, 424, 424 );
}

#endif // VGUI_FRAME_H
