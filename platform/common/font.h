// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef PLATFORM_COMMON_FONT_H
#define PLATFORM_COMMON_FONT_H
#include "font.h"
#include "fileimage.h"

namespace vgui
{
class SurfacePlat;

class BaseFontPlat
{
public:
	virtual ~BaseFontPlat() {}
	virtual bool equals( const char *, int, int, float, int, bool, bool, bool, bool ) = 0;
	virtual void getCharRGBA( int, int, int, int, int, unsigned char * ) = 0;
	virtual void getCharABCwide( int ch , int &a, int &b, int &c ) = 0;
	virtual int getTall() = 0;
	virtual int getWide() = 0;
	virtual void drawSetTextFont( SurfacePlat* ) = 0;
};

class VFontData
{
public:
	VFontData()
	{
		m_BitmapCharWidth = m_BitmapCharHeight = 0;
		m_pBitmap = NULL;
	}

	~VFontData()
	{
		if( m_pBitmap )
			delete[] m_pBitmap;
	}

	int m_CharWidths[256];
	int m_BitmapCharWidth;
	int m_BitmapCharHeight;
	unsigned char *m_pBitmap;
	int GetLineWidth();
};

class FontPlat_Bitmap : public BaseFontPlat
{
private:
	VFontData m_FontData;
	char *m_pName;

	FontPlat_Bitmap();
public:
	static FontPlat_Bitmap *Create( const char *, FileImageStream * );

	virtual ~FontPlat_Bitmap() override;
	virtual bool equals( const char *, int, int, float, int, bool, bool, bool, bool ) override;
	virtual void getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba ) override;
	virtual void getCharABCwide( int ch , int &a, int &b, int &c ) override;
	virtual int getTall() override;
	virtual int getWide() override;
	virtual void drawSetTextFont( SurfacePlat* ) override;
};
}

#endif // ATFORM_COMMON_FONT_H
