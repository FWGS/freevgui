// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "vgui.h"
#include "vgui_internal.h"
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

// as a debugging aid, setting FREEVGUI_NO_CUSTOM_ALLOCATORS to anything other than "0" makes the
// global operator new/delete overrides below go straight to the CRT, ignoring whatever was installed through vgui_setMalloc/vgui_setFree
static bool useInstalledAllocators()
{
	static int cached; // 0 = not yet determined, 1 = installed hooks, 2 = plain CRT

	if( !cached )
	{
		const char *env = getenv( "FREEVGUI_NO_CUSTOM_ALLOCATORS" );

		cached = ( env && strcmp( env, "0" )) ? 2 : 1;
	}

	return cached == 1;
}

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

void vgui::vgui_strcpy( char *dst, int size, const char *src )
{
	if( !dst || !src || !size )
		return;

	size_t len = strlen( src );

	if( len >= size ) // check if truncate
	{
		memcpy( dst, src, size );
		dst[size - 1] = 0;
	}
	else memcpy( dst, src, len + 1 );
}

char *vgui::vgui_strdup( const char *src )
{
	size_t len = strlen( src ) + 1;
	char *dst = new char[len];

	vgui_strcpy( dst, len, src );

	return dst;
}

void vgui_setMalloc( void *( *theMalloc )( size_t ))
{
	if( !theMalloc )
		staticMalloc = malloc;
	else staticMalloc = theMalloc;
}

void vgui_setFree( void ( *theFree )( void* ))
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
	return useInstalledAllocators() ? staticMalloc( size ) : malloc( size );
}

void* operator new[]( size_t size )
{
	return useInstalledAllocators() ? staticMalloc( size ) : malloc( size );
}

void operator delete( void* p )
{
	if( useInstalledAllocators() )
		staticFree( p );
	else free( p );
}

void operator delete[]( void* p )
{
	if( useInstalledAllocators() )
		staticFree( p );
	else free( p );
}

