// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_H
#define VGUI_H

#include "xash3d_mathlib.h"
#include "crtlib.h"

#ifdef XASH_64BIT
#define CHECK_STRUCT_SIZE( type, size ) // to be filled
#else
#define CHECK_STRUCT_SIZE( type, size ) \
	static_assert( sizeof( type ) == size, "invalid size" )
#endif

#if defined(__GNUC__)
	#define CLASSEXPORT __attribute__(( visibility( "default" )))
#else
	#define CLASSEXPORT EXPORT
#endif

#if defined( __GNUC__ )
	#define PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined( _MSC_VER )
	#define PRETTY_FUNCTION __FUNCSIG__
#else
	#define PRETTY_FUNCTION __func__
#endif

namespace vgui
{
int EXPORT vgui_printf( const char*, ... ) FORMAT_CHECK( 1 );
int EXPORT vgui_dprintf( const char*, ... ) FORMAT_CHECK( 1 );
int EXPORT vgui_dprintf2( const char*, ... ) FORMAT_CHECK( 1 );
void EXPORT vgui_strcpy( char*, int, const char* );
char EXPORT *vgui_strdup( const char* );
void EXPORT vgui_setMalloc( void* ( *theMalloc )( size_t ));
void EXPORT vgui_setFree( void ( *theFree )( void* ));

template <typename T>
class Dar
{
protected:
	int _count;
	int _capacity;
	T*  _data;
public:
	Dar( int capacity ) : _count( 0 ), _capacity( capacity ), _data( new T[capacity] ) {}
	Dar() : Dar( 4 ) {}

	void setCount( int count )
	{
		_count = bound( 0, count, _capacity );
	}

	int getCount() // can't make const due to ABI
	{
		return _count;
	}

	void ensureCapacity( int capacity )
	{
		T *newptr = new T[capacity];

		if( _data )
		{
			memcpy( newptr, _data, sizeof( T ) * _count );
			delete[] _data;
		}

		_data = newptr;
		_capacity = capacity;
	}

	void addElement( T element )
	{
		ensureCapacity( _count + 1 );
		_data[_count++] = element;
	}

	void putElement( T element )
	{
		if( !hasElement( element ))
			addElement( element );
	}

	bool hasElement( T element ) // can't make const due to ABI
	{
		int i = 0;

		for( ; i < _count; i++ )
		{
			if( _data[i] == element )
				break;
		}

		return i != _count;
	}

	void insertElementAt( T element, int at )
	{
		if( at < 0 || at > _count ) // specifically allow 0
			return;

		if( at == _count || !_count ) // simple case
		{
			addElement( element );
			return;
		}

		memmove( &_data[at + 1], &_data[at], sizeof( T ) * ( _count - at ));
		_data[at] = element;
		_count++;
	}

	void setElementAt( T element, int at )
	{
		if( at < 0 || at >= _count )
			return;
		_data[at] = element;
	}

	void removeElementAt( int at )
	{
		if( at < 0 || at >= _count )
			return;

		memmove( &_data[at], &_data[at + 1], sizeof( T ) * ( _count - at - 1 ));
		_count--;
	}

	void removeElement( T element )
	{
		for( int i = 0; i < _count; i++ )
		{
			if( _data[i] != element )
				continue;

			removeElementAt( i );
			break;
		}
	}

	void removeAll()
	{
		setCount( 0 );
	}

	T operator[]( int i ) { return _data[i]; }
};

CHECK_STRUCT_SIZE( Dar<void*>, 12 );

// kinda useless, as calls are inlined anyway but it exists as export in Windows build
#if _MSC_VER
class ActionSignal;
class Button;
class ChangeSignal;
class DesktopIcon;
class FocusChangeSignal;
class Frame;
class FrameSignal;
class InputSignal;
class IntChangeSignal;
class Label;
class Panel;
class RepaintSignal;
class SurfaceBase;
class TickSignal;

class CLASSEXPORT Dar<char>;
class CLASSEXPORT Dar<char*>;
class CLASSEXPORT Dar<Dar<char>>;
class CLASSEXPORT Dar<int>;
class CLASSEXPORT Dar<ActionSignal*>;
class CLASSEXPORT Dar<Button*>;
class CLASSEXPORT Dar<ChangeSignal*>;
class CLASSEXPORT Dar<DesktopIcon*>;
class CLASSEXPORT Dar<FocusChangeSignal*>;
class CLASSEXPORT Dar<Frame*>;
class CLASSEXPORT Dar<FrameSignal*>;
class CLASSEXPORT Dar<InputSignal*>;
class CLASSEXPORT Dar<IntChangeSignal*>;
class CLASSEXPORT Dar<Label*>;
class CLASSEXPORT Dar<Panel*>;
class CLASSEXPORT Dar<RepaintSignal*>;
class CLASSEXPORT Dar<SurfaceBase*>;
class CLASSEXPORT Dar<TickSignal*>;
#endif
}

#endif // VGUI_H
