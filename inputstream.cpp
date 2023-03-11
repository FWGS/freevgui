// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "inputstream.h"

using namespace vgui;

DataInputStream::DataInputStream( InputStream *is ) :
	_is( is )
{

}

void DataInputStream::seekStart( bool &success )
{
	if( _is )
		_is->seekStart( success );
	else success = false;
}

void DataInputStream::seekRelative( int count, bool &success )
{
	if( _is )
		_is->seekRelative( count, success );
	else success = false;
}

void DataInputStream::seekEnd( bool &success )
{
	if( _is )
		_is->seekEnd( success );
	else success = false;
}

int DataInputStream::getAvailable( bool &success )
{
	if( _is )
		return _is->getAvailable( success );

	success = false;
	return 0;
}

void DataInputStream::readUChar( unsigned char *buf, int count, bool &success )
{
	if( _is )
		_is->readUChar( buf, count, success );
	else success = false;
}

unsigned char DataInputStream::readUChar( bool &success )
{
	if( _is )
		return _is->readUChar( success );

	success = false;
	return 0;
}

void DataInputStream::close( bool &success )
{
	if( _is )
		_is->close( success );
	else success = false;
}

void DataInputStream::close()
{
	bool success;
	close( success );
}

bool DataInputStream::readBool( bool &success )
{
	if( _is )
		return _is->readUChar( success );

	success = false;
	return false;
}

char DataInputStream::readChar( bool &success )
{
	if( _is )
		return _is->readUChar( success );

	success = false;
	return false;
}

ushort DataInputStream::readUShort( bool &success )
{
	ushort ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

short int DataInputStream::readShort( bool &success )
{
	short int ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

int DataInputStream::readInt( bool &success )
{
	int ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

uint DataInputStream::readUInt( bool &success )
{
	uint ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

long int DataInputStream::readLong( bool &success )
{
	long int ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

ulong DataInputStream::readULong( bool &success )
{
	ulong ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

float DataInputStream::readFloat( bool &success )
{
	float ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

double DataInputStream::readDouble( bool &success )
{
	double ret = 0;

	if( _is )
		_is->readUChar( (unsigned char *)( &ret ), sizeof( ret ), success );
	else success = false;

	return ret;
}

void DataInputStream::readLine( char *str, int count, bool &success )
{
	unsigned char ch = 0;

	if( !_is )
	{
		success = false;
		return;
	}

	if( count > 0 )
	{
		for( int i = 0; i < count; i++ )
		{
			str[i] = 0;

			_is->readUChar( &ch, sizeof( ch ), success );
			if( !success )
				return;

			if( ch == '\n' )
				return;

			str[i] = ch;
		}
	}

	while( true )
	{
		_is->readUChar( &ch, sizeof( ch ), success );
		if( !success )
			return;

		if( ch == '\n' )
			break;
	}
}

FileInputStream::FileInputStream( const char *name, bool textmode )
{
	_fp = fopen( name, textmode ? "rt" : "rb" );
}

void FileInputStream::seekStart( bool &success )
{
	if( _fp )
		success = fseek( _fp, 0, SEEK_SET ) != 0; // ???
	else success = false;
}

void FileInputStream::seekRelative( int count, bool &success )
{
	if( _fp )
	{
		// a1ba: VGUI1 is probably written by idiots
		success = fseek( _fp, SEEK_CUR, count ) != 0;
	}
	else success = false;
}

void FileInputStream::seekEnd( bool &success )
{
	if( _fp )
	{
		// a1ba: same mistake
		success = fseek( _fp, SEEK_END, 0 ) != 0;
	}
	else success = false;
}

int FileInputStream::getAvailable( bool &success )
{
	// a1ba: it was that hard to use ftell?
	success = false;
	return 0;
}

void FileInputStream::readUChar( unsigned char *buf, int count, bool &success )
{
	if( _fp )
		success = fread( buf, count, 1, _fp ) == 1;
	else success = false;
}

unsigned char FileInputStream::readUChar( bool &success )
{
	if( _fp )
	{
		unsigned char ch;
		success = fread( &ch, sizeof( ch ), 1, _fp ) == 1;
		return ch;
	}

	success = false;
	return 0;
}

void FileInputStream::close( bool &success )
{
	if( _fp )
		success = fclose( _fp ) == 0;
	success = false;
}

void FileInputStream::close()
{
	bool success;
	close( success );
}
