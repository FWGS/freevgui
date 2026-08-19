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
	char* _title;
	bool _internal, _sizeable, _moveable;
	Panel* _topGrip;
	Panel* _bottomGrip;
	Panel* _leftGrip;
	Panel* _rightGrip;
	Panel* _topLeftGrip;
	Panel* _topRightGrip;
	Panel* _bottomLeftGrip;
	Panel* _bottomRightGrip;
	Panel* _captionGrip;
	Panel* _client;
	Button* _trayButton;
	Button* _minimizeButton;
	Button* _maximizeButton;
	Button* _closeButton;
	Button* _menuButton;
	Dar<FrameSignal*> _frameSignalDar;
	Frame* _resizeable;
};
CHECK_STRUCT_SIZE( Frame, 272, 424, 424 );
}

#endif // VGUI_FRAME_H
