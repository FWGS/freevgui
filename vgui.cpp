// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "vgui.h"
#include "app.h"
#include "buildgroup.h"
#include "font.h"
#include "image.h"
#include "input.h"
#include "inputstream.h"
#include "layout.h"
#include "panel.h"
#include "scheme.h"
#include "surface.h"
#include "controls/button.h"
#include "controls/configwizard.h"
#include "controls/desktop.h"
#include "controls/edit.h"
#include "controls/frame.h"
#include "controls/header.h"
#include "controls/image.h"
#include "controls/label.h"
#include "controls/list.h"
#include "controls/menu.h"
#include "controls/messagebox.h"
#include "controls/progressbar.h"
#include "controls/scroll.h"
#include "controls/tab.h"
#include "controls/table.h"
#include "controls/text.h"
#include "controls/treefolder.h"
#include "controls/wizard.h"

#include <stdlib.h>

using namespace vgui;

static void *( *staticMalloc )( size_t size ) = malloc;
static void ( *staticFree )( void *block ) = free;

int vgui::vgui_printf( const char* fmt, ... )
{
	va_list va;

	va_start( va, fmt );
	int ret = vprintf( fmt, va );
	va_end( va );

	return ret;
}

int vgui::vgui_dprintf( const char* fmt, ... )
{
	va_list va;

	va_start( va, fmt );
	int ret = vfprintf( stderr, fmt, va );
	va_end( va );

	return ret;
}

int vgui::vgui_dprintf2( const char* fmt, ... )
{
	static int counter = 0;
	va_list va;

	fprintf( stderr, "%d:", counter++ );

	va_start( va, fmt );
	int ret = vfprintf( stderr, fmt, va );
	va_end( va );

	return ret;
}

void vgui::vgui_strcpy( char *dst, int len, const char *src )
{
	Q_strncpy( dst, src, len );
}

char *vgui::vgui_strdup( const char *src )
{
	size_t len = strlen( src ) + 1;
	char *dst = new char[len];

	Q_strncpy( dst, src, len );

	return dst;
}

void vgui::vgui_setMalloc( void *( *theMalloc )( size_t ))
{
	if( !theMalloc )
		staticMalloc = malloc;
	else staticMalloc = theMalloc;
}

void vgui::vgui_setFree( void ( *theFree )( void* ))
{
	if( !theFree )
		staticFree = free;
	else staticFree = theFree;
}

extern "C" const char *freevgui_version( void )
{
	return "FreeVGUI 0.1";
}

void* operator new( size_t size )
{
	return staticMalloc( size );
}

void* operator new[]( size_t size )
{
	return staticMalloc( size );
}

void operator delete( void* p )
{
	staticFree( p );
}

void operator delete[]( void* p )
{
	staticFree( p );
}

