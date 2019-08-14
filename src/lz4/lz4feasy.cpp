#include "stdafx.h"
#include "lz4feasy.h"

namespace YTSvrLib
{
	LZ4FEasy::LZ4FEasy()
	{
		m_dCtx = NULL;
		LZ4F_createDecompressionContext(&m_dCtx, LZ4F_VERSION);
	}

	LZ4FEasy::~LZ4FEasy()
	{
		LZ4F_freeDecompressionContext(m_dCtx);
	}

	int LZ4FEasy::LZ4F_compress(const char* src, char* dst, int srcSize, int dstSize)
	{
		return (int) LZ4F_compressFrame(dst, dstSize, src, srcSize, NULL);
	}

	int LZ4FEasy::LZ4F_decompress(const char* src, char* dst, int srcSize, int dstSize)
	{
		return LZ4FEasy::GetInstance()->LZ4F_decompress_internal(src, dst, srcSize, dstSize);
	}

	int LZ4FEasy::LZ4F_decompress_internal(const char* src, char* dst, int _srcSize, int _dstSize)
	{
		size_t srcSize = (size_t) _srcSize;
		size_t dstSize = (size_t) _dstSize;
		size_t result = 0;
		result = ::LZ4F_decompress(m_dCtx, dst, &dstSize, src, &srcSize, NULL);
		return (int) dstSize;
	}
}

