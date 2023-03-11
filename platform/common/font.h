#ifndef PLATFORM_COMMON_FONT_H
#define PLATFORM_COMMON_FONT_H
#include "font.h"
#include "utlrbtree.h"
#include "platform/common/fileimage.h"

class SurfacePlat;

namespace vgui
{
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

class FontPlat : public BaseFontPlat
{
public:
	int bufSize[2];
	unsigned char *buf;

	VFontData m_BitmapFont;
	bool m_bBitmapFont;

protected:
	char m_szName[32];
	int m_iTall;
	int m_iWeight;
	int m_iFlags;
	bool m_bAntiAliased;
	bool m_bRotary;
	bool m_bAdditive;
	int m_iDropShadowOffset;
	bool m_bUnderlined;
	int m_iOutlineSize;
	int m_iHeight;
	int m_iMaxCharWidth;
	int m_iAscent;

	struct abc_t
	{
		short int b;
		char a;
		char c;
	};

	struct abc_cache_t
	{
		wchar_t wch;
		abc_t abc;
	};

	CUtlRBTree<abc_cache_t, short unsigned int> m_ExtendedABCWidthsCache;
	int m_iScanLines;
	int m_iBlur;
	float *m_pGaussianDistribution;

	struct font_name_entry
	{
		char *m_OSSpecificName;
		uint8_t m_cbOSSpecificName;

		char *m_pchFriendlyName;

		bool operator<( const font_name_entry &a );
	};

	static CUtlRBTree<font_name_entry, int> m_FriendlyNameCache;
	static bool ms_bSetFriendlyNameCacheLessFunc;

public:
	FontPlat( const char *, int, int, float, int, bool, bool, bool, bool );
	virtual ~FontPlat();
	virtual bool equals( const char *, int, int, float, int, bool, bool, bool, bool );
	virtual void getCharRGBA( int, int, int, int, int, unsigned char * );
	virtual void getCharABCwide( int ch , int &a, int &b, int &c );
	virtual int getTall();
	virtual int getWide();
	virtual void drawSetTextFont( SurfacePlat* );

protected:
	void CreateFontList();
	static bool ExtendedABCWidthsCacheLessFunc( const abc_cache_t&, const abc_cache_t& );
	bool FontLessFunc( font_name_entry&, font_name_entry& );
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
