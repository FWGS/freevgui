// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_FONT_H
#define VGUI_FONT_H

#include "vgui.h"

namespace vgui
{
class BaseFontPlat;
void Font_Reset( void );

class CLASSEXPORT Font
{
	friend class Surface;

protected:
	const char *_name;
	BaseFontPlat *_plat;
	int _id;
public:
	Font( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol );
	Font( const char *name, void *pFileData, int fileDataLen, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol );
	virtual void init( const char *name, void *pFileData, int fileDataLen, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol );
	BaseFontPlat *getPlat();
	virtual void getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba );
	virtual void getCharABCwide( int ch, int &a, int &b, int &c );
	virtual void getTextSize( const char *str, int &wide, int &tall );
	virtual int getTall();
#ifndef _WIN32
	virtual int getWide();
#endif
	virtual int getId();
};

CHECK_STRUCT_SIZE( Font, 16 );

}

#endif // VGUI_FONT_H
