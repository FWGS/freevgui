#include "platform/common/font.h"
#include "vgui_internal.h"

using namespace vgui;

FontPlat_Bitmap::FontPlat_Bitmap()
{
	m_pName = nullptr;
}

FontPlat_Bitmap::~FontPlat_Bitmap()
{
}

bool LoadVFontDataFrom32BitTGA( FileImageStream *fp, VFontData *pData )
{
	FileImage fileImage;

	if( !Load32BitTGA( fp, &fileImage ))
		return false;

	pData->m_pBitmap = new unsigned char[fileImage.m_Width * fileImage.m_Height];
	if( !pData->m_pBitmap )
		return false;

	memset( pData->m_pBitmap, 0, fileImage.m_Width * fileImage.m_Height );

	pData->m_BitmapCharWidth = fileImage.m_Width / 256;
	pData->m_BitmapCharHeight = fileImage.m_Height;

	for( int i = 0; i < 256; i++ )
	{
		unsigned char *in = &fileImage.m_pData[i * pData->m_BitmapCharWidth * 4];
		unsigned char *out = &pData->m_pBitmap[i * pData->m_BitmapCharWidth];
		int rightX = 0;

		for( int y = 0; y < pData->m_BitmapCharHeight; y++ )
		{
			for( int x = 0; x < pData->m_BitmapCharWidth; x++ )
			{
				if( in[x * 4] ||
				     in[x * 4 + 1] ||
				     in[x * 4 + 2] ||
				     in[x * 4 + 3] )
				{
					out[x] = 1;
					if( x > rightX )
						rightX = x;
				}
				else
				{
					out[x] = 0;
				}
			}

			in += 256 * pData->m_BitmapCharWidth * 4;
			out += 256 * pData->m_BitmapCharWidth;
		}

		if( i == 32 )
			pData->m_CharWidths[i] = pData->m_BitmapCharWidth / 4;
		else
			pData->m_CharWidths[i] = rightX;
	}

	return true;
}

FontPlat_Bitmap *FontPlat_Bitmap::Create( const char *name, FileImageStream *stream )
{
	FontPlat_Bitmap *bitmap = new FontPlat_Bitmap();
	if( !bitmap )
		return nullptr;

	if( !LoadVFontDataFrom32BitTGA( stream, &bitmap->m_FontData ))
		goto cleanup_and_fail;

	bitmap->m_pName = vgui_strdup( name );
	if( !bitmap->m_pName )
		goto cleanup_and_fail;

	return bitmap;

cleanup_and_fail:
	delete bitmap;
	return nullptr;
}

bool FontPlat_Bitmap::equals( const char *, int, int, float, int, bool, bool, bool, bool )
{
	return false;
}

void FontPlat_Bitmap::getCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
	unsigned char *pSrcPos, *pOutPos;

	ch = bound( 0, ch, 256 );

	for( int y = 0; y < m_FontData.m_BitmapCharHeight; y++ )
	{
		pSrcPos = &m_FontData.m_pBitmap[m_FontData.m_BitmapCharWidth * ( ch + y * 256 )];
		for( int x = 0; x < m_FontData.m_BitmapCharWidth; x++ )
		{
			int outX = rgbaX + x, outY = rgbaY + y;
			if(( outX < rgbaWide ) && ( outY < rgbaTall ))
			{
				pOutPos = &rgba[( outY * rgbaWide + outX ) * 4];
				if( pSrcPos[x] != 0 )
					memset( pOutPos, 255, 4 );
				else
					memset( pOutPos, 0, 4 );
			}
		}
	}
}

void FontPlat_Bitmap::getCharABCwide( int ch, int &a, int &b, int &c )
{
	ch = bound( 0, ch, 256 );

	a = c = 0;
	b = m_FontData.m_CharWidths[ch] + 1;
}

int FontPlat_Bitmap::getTall()
{
	return m_FontData.m_BitmapCharHeight;
}

int FontPlat_Bitmap::getWide()
{
	return m_FontData.m_BitmapCharWidth;
}

void FontPlat_Bitmap::drawSetTextFont( SurfacePlat *plat )
{
}
