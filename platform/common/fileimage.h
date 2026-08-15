// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef PLATFORM_COMMON_FILEIMAGE_H
#define PLATFORM_COMMON_FILEIMAGE_H
#include <stdio.h>
#include "vgui.h"

class FileImage
{
public:
	int m_Width;
	int m_Height;
	unsigned char *m_pData;

	FileImage() { Clear(); }
	~FileImage() { Term(); }

	void Term()
	{
		if( m_pData )
			delete[] m_pData;
		Clear();
	}

	void Clear()
	{
		m_Width = m_Height = 0;
		m_pData = NULL;
	}
};

#pragma pack( push, 1 )
struct TGAFileHeader
{
	uint8_t m_IDLength;
	uint8_t m_ColorMapType;
	uint8_t m_ImageType;
	uint16_t m_CMapStart;
	uint16_t m_CMapLength;
	uint8_t m_CMapDepth;
	uint16_t m_XOffset;
	uint16_t m_YOffset;
	uint16_t m_Width;
	uint16_t m_Height;
	uint8_t m_PixelDepth;
	uint8_t m_ImageDescriptor;
};
#pragma pack( pop )

class FileImageStream
{
public:
	virtual void Read( void *, int ) = 0;
	virtual bool ErrorStatus() = 0;
};

class FileImageStream_Memory : public FileImageStream
{
private:
	unsigned char *m_pData;
	int m_DataLen;
	int m_CurPos;
	bool m_bError;
public:
	FileImageStream_Memory( void *pData, int dataLen )
	{
		m_pData = (unsigned char *)pData;
		m_DataLen = dataLen;
		m_CurPos = 0;
		m_bError = false;
	}

	virtual void Read( void *pData, int len )
	{
		unsigned char *data = (unsigned char *)pData;

		for( int i = 0; i < len; i++ )
		{
			if( m_CurPos < m_DataLen )
			{
				data[i] = m_pData[m_CurPos];
				m_CurPos++;
			}
			else
			{
				data[i] = 0;
				m_bError = true;
			}
		}
	}

	virtual bool ErrorStatus()
	{
		bool error = m_bError;
		m_bError = false;
		return error;
	}
};

typedef void *FileHandle_t;

bool Load32BitTGA( FileImageStream *fp, FileImage *image );
void Save32BitTGA( FileHandle_t fp, FileImage *image );

#endif // PLATFORM_COMMON_FILEIMAGE_H
