// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_INPUTSTREAM_H
#define VGUI_INPUTSTREAM_H

#include <stdio.h>
#include "vgui.h"

namespace vgui
{
class CLASSEXPORT InputStream
{
public:
	virtual void seekStart( bool &success ) = 0;
	virtual void seekRelative( int count, bool &success ) = 0;
	virtual void seekEnd( bool &success ) = 0;
	virtual int getAvailable( bool &success ) = 0;
	virtual unsigned char readUChar( bool &success ) = 0;
	virtual void readUChar( unsigned char *buf, int count, bool &success ) = 0;
	virtual void close( bool &success ) = 0;
};

// "virtual public" here is not a typo, the inheritance is virtual in the original
class CLASSEXPORT DataInputStream : virtual public InputStream
{
private:
	InputStream *stream;
public:
	DataInputStream( InputStream * );

	virtual void seekStart( bool &success ) override;
	virtual void seekRelative( int, bool &success ) override;
	virtual void seekEnd( bool &success ) override;
	virtual int getAvailable( bool &success ) override;
	virtual void readUChar( unsigned char *buf, int count, bool &success ) override;
	virtual void close( bool &success ) override;
	virtual void close();
	virtual bool readBool( bool &success );
	virtual char readChar( bool &success );
	virtual unsigned char readUChar( bool &success ) override;
	virtual short int readShort( bool &success );
	virtual unsigned short readUShort( bool &success );
	virtual int readInt( bool &success );
	virtual unsigned int readUInt( bool &success );
	virtual long int readLong( bool &success );
	virtual unsigned long readULong( bool &success );
	virtual float readFloat( bool &success );
	virtual double readDouble( bool &success );
	virtual void readLine( char *str, int count, bool &success );
};
CHECK_STRUCT_SIZE( DataInputStream, 8, 16, 16 );

class CLASSEXPORT FileInputStream : public InputStream
{
private:
	FILE *fp;
public:
	FileInputStream( const char *name, bool textmode );
	virtual void seekStart( bool &success ) override;
	virtual void seekRelative( int, bool &success ) override;
	virtual void seekEnd( bool &success ) override;
	virtual int getAvailable( bool &success ) override;
	virtual unsigned char readUChar( bool &success ) override;
	virtual void readUChar( unsigned char *buf, int count, bool &success ) override;
	virtual void close( bool &success ) override;
	virtual void close();
};
CHECK_STRUCT_SIZE( FileInputStream, 8, 16, 16 );

}

#endif // VGUI_INPUT_H
