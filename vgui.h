// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_H
#define VGUI_H

#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#if defined( __GNUC__ )
	#if defined( __i386__ )
		#define EXPORT __attribute__(( visibility( "default" ), force_align_arg_pointer ))
	#else
		#define EXPORT __attribute__(( visibility ( "default" )))
	#endif
	#define FORMAT_CHECK( x ) __attribute__(( format( printf, x, x + 1 )))
#elif defined( _MSC_VER )
	#define EXPORT __declspec( dllexport )
#endif

#if !defined( EXPORT )
	#define EXPORT
#endif

#if !defined( FORMAT_CHECK )
	#define FORMAT_CHECK( x )
#endif

#if defined( __LP64__ ) || defined( _LP64 )
	#define CHECK_STRUCT_SIZE( type, sizeIlp32, sizeLp64, sizeLlp64 ) static_assert( sizeof( type ) == sizeLp64, "invalid size" )
#elif XASH_64BIT || defined( _WIN64 )
	#define CHECK_STRUCT_SIZE( type, sizeIlp32, sizeLp64, sizeLlp64 ) static_assert( sizeof( type ) == sizeLlp64, "invalid size" )
#else
	#define CHECK_STRUCT_SIZE( type, sizeIlp32, sizeLp64, sizeLlp64 ) static_assert( sizeof( type ) == sizeIlp32, "invalid size" )
#endif

// for a class that only overrides virtuals and must stay exactly as big as its base
#define CHECK_STRUCT_SIZE_EQ( type, other ) static_assert( sizeof( type ) == sizeof( other ), "invalid size" )

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
	int count;
	int capacity;
	T*  data;
public:
	Dar( int capacity ) : count( 0 ), capacity( capacity ), data( new T[capacity] ) {}
	Dar() : Dar( 4 ) {}

	void setCount( int newCount )
	{
		count = newCount >= 0 ? newCount < capacity ? newCount : capacity : 0;
	}

	int getCount() // can't make const due to ABI
	{
		return count;
	}

	void ensureCapacity( int newCapacity )
	{
		T *newptr = new T[newCapacity];

		if( data )
		{
			memcpy( newptr, data, sizeof( T ) * count );
			delete[] data;
		}

		data = newptr;
		capacity = newCapacity;
	}

	void addElement( T element )
	{
		ensureCapacity( count + 1 );
		data[count++] = element;
	}

	void putElement( T element )
	{
		if( !hasElement( element ))
			addElement( element );
	}

	bool hasElement( T element ) // can't make const due to ABI
	{
		int i = 0;

		for( ; i < count; i++ )
		{
			if( data[i] == element )
				break;
		}

		return i != count;
	}

	void insertElementAt( T element, int at )
	{
		if( at < 0 || at > count ) // specifically allow 0
			return;

		if( at == count || !count ) // simple case
		{
			addElement( element );
			return;
		}

		memmove( &data[at + 1], &data[at], sizeof( T ) * ( count - at ));
		data[at] = element;
		count++;
	}

	void setElementAt( T element, int at )
	{
		if( at < 0 || at >= count )
			return;
		data[at] = element;
	}

	void removeElementAt( int at )
	{
		if( at < 0 || at >= count )
			return;

		memmove( &data[at], &data[at + 1], sizeof( T ) * ( count - at - 1 ));
		count--;
	}

	void removeElement( T element )
	{
		for( int i = 0; i < count; i++ )
		{
			if( data[i] != element )
				continue;

			removeElementAt( i );
			break;
		}
	}

	void removeAll()
	{
		setCount( 0 );
	}

	T operator[]( int i ) { return data[i]; }

	// not in the original interface: modern compilers instantiate every member of the
	// exported instantiations below, and Dar<Dar<char>> needs element comparison for that
	bool operator==( const Dar &other ) const
	{
		if( count != other.count )
			return false;

		for( int i = 0; i < count; i++ )
		{
			if( !( data[i] == other.data[i] ))
				return false;
		}

		return true;
	}

	bool operator!=( const Dar &other ) const { return !( *this == other ); }

	T *begin() { return data; }
	T *end() { return data + count; }
	const T *begin() const { return data; }
	const T *end() const { return data + count; }
};

CHECK_STRUCT_SIZE( Dar<void*>, 12, 16, 16 );

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

// explicit instantiation definitions, exported from the DLL for ABI compatibility with the original vgui.dll (modern syntax for the old `class CLASSEXPORT Dar<T>;` MSVC6-ism)
template class CLASSEXPORT Dar<char>;
template class CLASSEXPORT Dar<char*>;
template class CLASSEXPORT Dar<Dar<char>>;
template class CLASSEXPORT Dar<int>;
template class CLASSEXPORT Dar<ActionSignal*>;
template class CLASSEXPORT Dar<Button*>;
template class CLASSEXPORT Dar<ChangeSignal*>;
template class CLASSEXPORT Dar<DesktopIcon*>;
template class CLASSEXPORT Dar<FocusChangeSignal*>;
template class CLASSEXPORT Dar<Frame*>;
template class CLASSEXPORT Dar<FrameSignal*>;
template class CLASSEXPORT Dar<InputSignal*>;
template class CLASSEXPORT Dar<IntChangeSignal*>;
template class CLASSEXPORT Dar<Label*>;
template class CLASSEXPORT Dar<Panel*>;
template class CLASSEXPORT Dar<RepaintSignal*>;
template class CLASSEXPORT Dar<SurfaceBase*>;
template class CLASSEXPORT Dar<TickSignal*>;
#endif
}

// my extension
extern "C" {
const char EXPORT *freevgui_version( void );
}

#endif // VGUI_H
