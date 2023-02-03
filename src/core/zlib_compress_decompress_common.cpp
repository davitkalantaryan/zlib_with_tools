
// zlib_compress_decompress_common.cpp
// 2018 Feb 12

#include <zlib_with_tools/zlib_compress_decompress_common.h>
#include <stdlib.h>
#include <search.h>
#include <memory.h>

CPPUTILS_BEGIN_C

ZLIBANDTLS_EXPORT SCompressDecompressHeader* ZlibCreateCompressDecompressHeader(uint32_t a_headerSize, uint32_t a_typeOfCompressedContent,uint32_t a_numberOfItems)
{
	SCompressDecompressHeader* pCompressDecompressHeader = (SCompressDecompressHeader*)malloc(a_headerSize);
	if (!pCompressDecompressHeader) { return pCompressDecompressHeader; }
	pCompressDecompressHeader->version = ZLIB_COMPR_DECOMPR_VERSION;
	pCompressDecompressHeader->numberOfItems = a_numberOfItems;
	pCompressDecompressHeader->wholeHeaderSizeInBytes = a_headerSize;
	pCompressDecompressHeader->typeOfCompressedContent = a_typeOfCompressedContent;
	return pCompressDecompressHeader;
}


ZLIBANDTLS_EXPORT SCompressDecompressHeader* ZlibCreateAndCopyComprDecomprHeader(const SCompressDecompressHeader* a_orgin, int a_nAll)
{
	SCompressDecompressHeader* pCompressDecompressHeader = (SCompressDecompressHeader*)malloc(a_orgin->wholeHeaderSizeInBytes);
	size_t unSize = a_nAll ? a_orgin->wholeHeaderSizeInBytes : sizeof(SCompressDecompressHeader);

	if (!pCompressDecompressHeader) { return pCompressDecompressHeader; }
	memcpy(pCompressDecompressHeader, a_orgin, unSize);
	return pCompressDecompressHeader;
}


ZLIBANDTLS_EXPORT void DestroyCompressDecompressHeader(SCompressDecompressHeader* a_header)
{
	free(a_header);
}


CPPUTILS_END_C
