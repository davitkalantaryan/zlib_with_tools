//
// file:			zlibwt_decompress_data.h
// path:			src/private/include/zlib_with_tools/zlibwt_decompress_data.h
// created on:		2023 Feb 11
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_DATA_H
#define ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_DATA_H

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/zlibwt_decompress_data.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>
#include <zlib.h>


CPPUTILS_BEGIN_C

struct CPPUTILS_DLL_PRIVATE ZlibWtLLDecompressSession {
    z_stream                        z_str;
    ZlibWtTypeLLDecompressCallback  clbk;
    void*                           userData;
    Bytef*                          bufferForDecompressedData;
    uInt                            sizeForBufferForDecompressedData;
    uInt                            reserved01;
    //SCompressDecompressHeader       header;
};


struct CPPUTILS_DLL_PRIVATE ZlibWtDecompressSession {
    struct ZlibWtLLDecompressSession        ll;
    struct SCompressDecompressHeader        header;
    struct SFileItem                        fileItem;
    DirIterFileData                         fileData;
    struct SZlibWtDecompressDirCallbacks    clbks;
    void*                                   userData;
    Bytef*                                  bufferForDecompressedDataTmp;
    size_t                                  reserved01;
    uInt                                    sizeForBufferForDecompressedDataTmp;
    uInt                                    isInited : 1;
    uInt                                    reserved02 : 31;
};

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_DATA_H
