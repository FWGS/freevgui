// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "fileimage.h"
#include "vgui_internal.h"

bool Load32BitTGA( FileImageStream *fp, FileImage *image )
{
	TGAFileHeader hdr;

	image->Term();

	fp->Read( &hdr, sizeof( hdr ));
	if( fp->ErrorStatus() || hdr.m_PixelDepth != 32 || hdr.m_ImageType != 10 )
		return false;

	for( int i = 0; i < hdr.m_IDLength; i++ )
	{
		unsigned char data;
		fp->Read( &data, sizeof( data ));
	}

	if( fp->ErrorStatus())
		return false;

	image->m_Width = hdr.m_Width;
	image->m_Height = hdr.m_Height;
	image->m_pData = new unsigned char[image->m_Width * image->m_Height * 4];
	if( !image->m_pData )
		return false;

	for( int y = image->m_Height - 1; y >= 0; y-- )
	{
		uint8_t *line = &image->m_pData[y * image->m_Width * 4];
		int curOut = 0;
		while( curOut < image->m_Width )
		{
			uint8_t packetHeader;
			fp->Read( &packetHeader, sizeof( packetHeader ));

			if( fp->ErrorStatus())
				return false;

			int runLength = (int)(packetHeader & 0x7f ) + 1;

			if( curOut + runLength > image->m_Width )
				return false;

			if( packetHeader & 0x80 )
			{
				uint32_t color;
				fp->Read( &color, sizeof( color ));

				if( fp->ErrorStatus())
					return false;

				for( int x = 0; x < runLength; x++ )
				{
					memcpy( line, &color, sizeof( color ));
					line += 4;
				}
			}
			else
			{
				for( int x = 0; x < runLength; x++ )
				{
					fp->Read( line, sizeof( uint32_t ));

					if( fp->ErrorStatus())
						return false;

					line += 4;
				}
			}

			curOut += runLength;
		}
	}

	return true;
}

static void WriteRun( unsigned char *pColor, FILE *fp, int runLength )
{
	uint8_t runCount = ( runLength - 1 ) | 0x80;

	fwrite( &runCount, 1, 1, fp );
	fwrite( pColor, 1, 4, fp );
}

void Save32BitTGA( FileHandle_t fp, FileImage *image )
{
	TGAFileHeader hdr = {};

	hdr.m_PixelDepth = 32;
	hdr.m_ImageType = 10;
	hdr.m_Width = image->m_Width;
	hdr.m_Height = image->m_Height;

	fwrite( &hdr, 1, sizeof( hdr ), (FILE *)fp );

	for( int y = image->m_Height - 1; y >= 0; y-- )
	{
		uint8_t *line = &image->m_pData[y * image->m_Width * 4];
		int runStart = 0, x;

		for( x = 0; x < image->m_Width; x++ )
		{
			if(( x - runStart ) >= 128 || *(( uint32_t* )&line[runStart * 4] ) != *(( uint32_t* )&line[x * 4] ))
			{
				WriteRun( &line[runStart * 4], (FILE *)fp, x - runStart );
				runStart = x;
			}
		}

		if( x - runStart > 0 )
		{
			WriteRun( &line[runStart * 4], (FILE *)fp, x - runStart );
		}
	}
}
