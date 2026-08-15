// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

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

// ====== FreeVGUI extensions ======

template <typename F>
class LambdaActionSignal : public ActionSignal
{
	F func;
public:
	LambdaActionSignal( F f ) : func( f )
	{
	}

	void actionPerformed( Panel *panel ) override
	{
		func( panel );
	}
};

template <typename F>
LambdaActionSignal<F> *makeActionHandler( F func )
{
	return new LambdaActionSignal<F>( func );
}

template <typename F>
class LambdaChangeSignal : public ChangeSignal
{
	F func;
public:
	LambdaChangeSignal( F f ) : func( f )
	{
	}

	void valueChanged( Panel *panel ) override
	{
		func( panel );
	}
};

template <typename F>
LambdaChangeSignal<F> *makeChangeHandler( F func )
{
	return new LambdaChangeSignal<F>( func );
}

template <typename F>
class LambdaIntChangeSignal : public IntChangeSignal
{
	F func;
public:
	LambdaIntChangeSignal( F f ) : func( f )
	{
	}

	void intChanged( int value, Panel *panel ) override
	{
		func( value, panel );
	}
};

template <typename F>
LambdaIntChangeSignal<F> *makeIntChangeHandler( F func )
{
	return new LambdaIntChangeSignal<F>( func );
}

template <typename F>
class LambdaFocusChangeSignal : public FocusChangeSignal
{
	F func;
public:
	LambdaFocusChangeSignal( F f ) : func( f )
	{
	}

	void focusChanged( bool state, Panel *panel ) override
	{
		func( state, panel );
	}
};

template <typename F>
LambdaFocusChangeSignal<F> *makeFocusChangeHandler( F func )
{
	return new LambdaFocusChangeSignal<F>( func );
}

template <typename F>
class LambdaRepaintSignal : public RepaintSignal
{
	F func;
public:
	LambdaRepaintSignal( F f ) : func( f )
	{
	}

	void panelRepainted( Panel *panel ) override
	{
		func( panel );
	}
};

template <typename F>
LambdaRepaintSignal<F> *makeRepaintHandler( F func )
{
	return new LambdaRepaintSignal<F>( func );
}

template <typename F>
class LambdaTickSignal : public TickSignal
{
	F func;
public:
	LambdaTickSignal( F f ) : func( f )
	{
	}

	void ticked() override
	{
		func();
	}
};

template <typename F>
LambdaTickSignal<F> *makeTickHandler( F func )
{
	return new LambdaTickSignal<F>( func );
}

class InputSignalAdapter : public InputSignal
{
public:
	void cursorMoved( int, int, Panel * ) override {}
	void cursorEntered( Panel * ) override {}
	void cursorExited( Panel * ) override {}
	void mousePressed( MouseCode, Panel * ) override {}
	void mouseDoublePressed( MouseCode, Panel * ) override {}
	void mouseReleased( MouseCode, Panel * ) override {}
	void mouseWheeled( int, Panel * ) override {}
	void keyPressed( KeyCode, Panel * ) override {}
	void keyTyped( KeyCode, Panel * ) override {}
	void keyReleased( KeyCode, Panel * ) override {}
	void keyFocusTicked( Panel * ) override {}
};
}

#endif // VGUI_SINGALS_H
