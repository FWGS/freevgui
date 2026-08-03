#include "font.h"
#include "vgui_internal.h"
#include "platform/common/font.h"
#include "platform/common/fileimage.h"

using namespace vgui;

FontPlat::FontPlat( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol ) :
	m_ExtendedABCWidthsCache( 256, 0, &ExtendedABCWidthsCacheLessFunc )
{
	m_bBitmapFont = false;

	vgui_strcpy( m_szName, sizeof( m_szName ), name );
	m_bAntiAliased = false;
	m_bRotary = false;
	m_bAdditive = false;
	m_iTall = tall;
	m_iWeight = weight;
	m_iFlags = 0;
	m_bUnderlined = underline;
	m_iDropShadowOffset = 0;
	m_iOutlineSize = 0;
	m_iBlur = 0;
	m_iScanLines = 0;
}

FontPlat::~FontPlat()
{

}

bool FontPlat::ExtendedABCWidthsCacheLessFunc( const abc_cache_t &lhs, const abc_cache_t &rhs )
{
	return lhs.wch < rhs.wch;
}

bool FontPlat::equals( const char *name, int tall, int wide, float rotation, int weight, bool italic, bool underline, bool strikeout, bool symbol )
{
	return !strcmp( name, m_szName ) && m_iTall == tall && m_iWeight == weight && m_bUnderlined == underline;
}

void FontPlat::getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
}

void FontPlat::getCharABCwide( int ch, int &a, int &b, int &c )
{
	abc_cache_t finder = { ch };

	unsigned short i = m_ExtendedABCWidthsCache.Find( finder );
	if( m_ExtendedABCWidthsCache.IsValidIndex( i ))
	{
		a = m_ExtendedABCWidthsCache[i].abc.a;
		b = m_ExtendedABCWidthsCache[i].abc.b;
		c = m_ExtendedABCWidthsCache[i].abc.c;
		return;
	}
}

int FontPlat::getTall()
{
	return m_iHeight;
}

int FontPlat::getWide()
{
	return m_iMaxCharWidth;
}

void FontPlat::drawSetTextFont( SurfacePlat * )
{

}

void FontPlat::CreateFontList()
{

}
