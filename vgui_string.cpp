// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#if FREEVGUI_FIXES
#include <climits>
#endif
#include "vgui_string.h"

using namespace vgui;

String::String()
{
#if FREEVGUI_FIXES
	text = 0;
#else
	// NOTE: no allocation here, the empty string is a literal
	text = (char *)"";
#endif
}

String::String( const char *newText )
{
#if FREEVGUI_FIXES
	int count = getCount( newText );
	text = 0;
	if( count )
	{
		text = new char[sizeof( StringHeader ) + count + 1];
		StringHeader *strhdr = reinterpret_cast<StringHeader*>( text );
		strhdr->size = count;
		strhdr->refcount = 1;
		text += sizeof( StringHeader );
		memcpy( text, newText, count + 1 );
	}
#else
	int count = getCount( newText );

	text = new char[count + 1];
	memcpy( text, newText, count + 1 );
#endif
}

String::String( const String &src )
{
#if FREEVGUI_FIXES
	text = 0;
	if( src.text )
	{
		reinterpret_cast<StringHeader*>( text )->refcount++;
		text = src.text;
	}
#else
	int count = getCount( src.text );

	text = new char[count + 1];
	memcpy( text, src.text, count + 1 );
#endif
}

String::~String()
{
#if FREEVGUI_FIXES
	if( text && 0 == --(reinterpret_cast<StringHeader*>( text ))->refcount)
	{
		text -= sizeof( StringHeader );
		delete[] text;
		text = 0;
	}
#endif
	// NOTE: text is intentionally not freed here, the original leaks it
}

int String::getCount( const char *str )
{
#if FREEVGUI_FIXES
	// avoid possible signed integer overflow here
	return static_cast<int>( strlen( str ) & INT_MAX );
#else
	return static_cast<int>( strlen( str ));
#endif
}

int String::getCount()
{
#if FREEVGUI_FIXES
	return text ? ( reinterpret_cast<StringHeader*>( text )[-1] ).size : 0;
#else
	return getCount( text );
#endif
}

String String::operator+( String other )
{
#if FREEVGUI_FIXES
	if( !text ) return other;
	if( !other.text ) return *this;

	String ret;
	size_t count = getCount();
	size_t otherCount = other.getCount();
	int newcount = ( count + otherCount ) & INT_MAX;

	ret.text = new char[sizeof( StringHeader ) + newcount + 1];

	StringHeader *strhdr = reinterpret_cast<StringHeader*>( ret.text );
	strhdr->size = newcount;
	strhdr->refcount = 1;

	ret.text += sizeof( StringHeader );

	memcpy( ret.text, text, count );
	memcpy( ret.text + count, other.text, newcount - count );
	ret.text[newcount] = '\0';

	return ret;
#else
	return *this + other.text;
#endif
}

String String::operator+( const char *other )
{
#if FREEVGUI_FIXES
	if( !text ) return other;
	if( !(other || *other )) return *this;

	String ret;
	size_t count = getCount();
	size_t otherCount = getCount( other );
	int newcount = ( count + otherCount ) & INT_MAX;

	ret.text = new char[sizeof( StringHeader ) + newcount + 1];

	StringHeader *strhdr = reinterpret_cast<StringHeader*>( ret.text );
	strhdr->size = newcount;
	strhdr->refcount = 1;

	ret.text += sizeof( StringHeader );

	memcpy( ret.text, text, count );
	memcpy( ret.text + count, other, newcount - count );
	ret.text[newcount] = '\0';

	return ret;
#else
	int count = getCount();
	int otherCount = getCount( other );
	char *buf = new char[count + otherCount + 1];

	memcpy( buf, text, count );
	memcpy( buf + count, other, otherCount + 1 );

	String ret( buf );

	delete[] buf;

	return ret;
#endif
}

bool String::operator==( String other )
{
#if FREEVGUI_FIXES
	if( text == other.text ) return true;
	if( getChars( ) != other.getChars( )) return false;
	return !strcmp( text, other.text );
#else
	return *this == other.text;
#endif
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
#if FREEVGUI_FIXES
	return text ? text : "";
#else
	return text;
#endif
}

void String::test()
{
}
