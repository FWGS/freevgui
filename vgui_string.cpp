// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "vgui_string.h"

using namespace vgui;

String::String()
{
	// NOTE: no allocation here, the empty string is a literal
	text = (char *)"";
}

String::String( const char *newText )
{
	int count = getCount( newText );

	text = new char[count + 1];
	memcpy( text, newText, count + 1 );
}

String::String( const String &src )
{
	int count = getCount( src.text );

	text = new char[count + 1];
	memcpy( text, src.text, count + 1 );
}

String::~String()
{
	// NOTE: text is intentionally not freed here, the original leaks it
}

int String::getCount( const char *str )
{
	return (int)strlen( str );
}

int String::getCount()
{
	return getCount( text );
}

String String::operator+( String other )
{
	return *this + other.text;
}

String String::operator+( const char *other )
{
	int count = getCount();
	int otherCount = getCount( other );
	char *buf = new char[count + otherCount + 1];

	memcpy( buf, text, count );
	memcpy( buf + count, other, otherCount + 1 );

	String ret( buf );

	delete[] buf;

	return ret;
}

bool String::operator==( String other )
{
	return *this == other.text;
}

bool String::operator==( const char *other )
{
	return !strcmp( text, other );
}

char String::operator[]( int index )
{
	return text[index];
}

const char *String::getChars()
{
	return text;
}

void String::test()
{
}
