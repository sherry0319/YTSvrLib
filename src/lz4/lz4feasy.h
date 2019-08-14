#pragma once

#include "lz4frame.h"

namespace YTSvrLib
{
	class YTSVRLIB_EXPORT LZ4FEasy : public CSingle<LZ4FEasy>
	{
	public:
		LZ4FEasy();
		virtual ~LZ4FEasy();

		static int LZ4F_compress(const char* src, char* dst, int srcSize, int dstSize);

		static int LZ4F_decompress(const char* src, char* dst, int srcSize, int dstSize);
		int LZ4F_decompress_internal(const char* src, char* dst, int srcSize, int dstSize);
	private:
		LZ4F_decompressionContext_t m_dCtx;
	};
}
