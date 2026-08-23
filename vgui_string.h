// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

// NOTE: named vgui_string.h and not string.h, because the source directory is
// on the include path, where it would shadow libc's <string.h>

#ifndef VGUI_STRING_H
#define VGUI_STRING_H

#include "vgui.h"

namespace vgui
{
// NOTE: non-polymorphic, must stay exactly pointer-sized. Never add virtuals here
class CLASSEXPORT String
{
private:
	char *text;

	int getCount( const char *str );
public:
	String();
	String( const char *newText );
	String( const String &src );
	~String();

	int getCount();
	String operator+( String other );
	String operator+( const char *other );
	bool operator==( String other );
	bool operator==( const char *other );
	char operator[]( int index );
	const char *getChars();
	static void test();
};

CHECK_STRUCT_SIZE( String, 4, 8, 8 );

}

#endif // VGUI_STRING_H
