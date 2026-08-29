// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "support.h"

using namespace vgui;

vguiapi_t *vgui::g_engine = nullptr;
Panel *vgui::g_rootPanel = nullptr;
XashSurface *vgui::g_surface = nullptr;

// declare as global so it installs itself as App::getInstance singleton
static XashApp staticApp;

static void XashStartup( int width, int height )
{
	if( g_rootPanel )
	{
		g_rootPanel->setSize( width, height );
		return;
	}

	g_rootPanel = new Panel( 0, 0, width, height );
	g_rootPanel->setPaintBorderEnabled( false );
	g_rootPanel->setPaintBackgroundEnabled( false );
	g_rootPanel->setVisible( true );
	g_rootPanel->setCursor( new Cursor( Cursor::DC_NONE ));

	staticApp.start();
	staticApp.setMinimumTickMillisInterval( 0 );

	g_surface = new XashSurface( g_rootPanel );
	g_rootPanel->setSurfaceBaseTraverse( g_surface );
	g_engine->DrawInit();
}

static void XashShutdown( void )
{
	staticApp.stop();

	delete g_rootPanel;
	g_rootPanel = nullptr;

	delete g_surface;
	g_surface = nullptr;
}

static void *XashGetPanel( void )
{
	return (void *)g_rootPanel;
}

static void XashPaint( void )
{
	if( !g_engine->IsInGame() || !g_rootPanel || !g_surface )
		return;

	Panel *panel = g_surface->getPanel();

	if( !panel )
		return;

	int width, height;

	g_rootPanel->getSize( width, height );

	g_scissor.enable();

	staticApp.externalTick();

	panel->setBounds( 0, 0, width, height );
	panel->repaint();
	panel->paintTraverse();

	g_scissor.disable();
}

#if defined( INTERNAL_VGUI_SUPPORT )
// what the engine probes in the client library when we're linked into it statically
#define VGUI_SUPPORT_INIT InitVGUISupportAPI
#else
#define VGUI_SUPPORT_INIT InitAPI
#endif

extern "C" void EXPORT VGUI_SUPPORT_INIT( vguiapi_t *api )
{
	// the structure is owned by the engine, keep the pointer, not a copy
	g_engine = api;

	api->Startup = XashStartup;
	api->Shutdown = XashShutdown;
	api->GetPanel = XashGetPanel;
	api->Paint = XashPaint;
	api->Mouse = XashMouse;
	api->Key = XashKey;
	api->MouseMove = XashMouseMove;
	api->TextInput = XashTextInput;
}
