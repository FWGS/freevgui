// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "support.h"

using namespace vgui;

void vgui::XashKey( VGUI_KeyAction action, VGUI_KeyCode code )
{
	if( !g_surface )
		return;

	App *app = App::getInstance();

	// VGUI_KeyCode is laid out to match vgui::KeyCode exactly
	switch( action )
	{
	case KA_PRESSED:
		app->internalKeyPressed( (KeyCode)code, g_surface );
		break;
	case KA_RELEASED:
		app->internalKeyReleased( (KeyCode)code, g_surface );
		break;
	case KA_TYPED:
		app->internalKeyTyped( (KeyCode)code, g_surface );
		break;
	}
}

void vgui::XashMouse( VGUI_MouseAction action, int code )
{
	if( !g_surface )
		return;

	App *app = App::getInstance();

	// VGUI_MouseCode is laid out to match vgui::MouseCode exactly
	switch( action )
	{
	case MA_PRESSED:
		app->internalMousePressed( (MouseCode)code, g_surface );
		break;
	case MA_RELEASED:
		app->internalMouseReleased( (MouseCode)code, g_surface );
		break;
	case MA_DOUBLE:
		app->internalMouseDoublePressed( (MouseCode)code, g_surface );
		break;
	case MA_WHEEL:
		app->internalMouseWheeled( code, g_surface ); // here the code is a signed wheel delta, not a button
		break;
	}
}

void vgui::XashMouseMove( int x, int y )
{
	if( !g_surface )
		return;

	// already scaled out of physical pixels by the engine
	App::getInstance()->internalCursorMoved( x, y, g_surface );
}

void vgui::XashTextInput( const char * )
{
	// stub, VGUI has no concept of text input
}
