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
#include <zlib_with_tools/zlibwt_zlib_h_wrapper.h>
#include <private/zlib_with_tools/zlibwt_compress_decompress_common.h>


CPPUTILS_BEGIN_C

#define ZLIBWT_DECOMPR_STAGE_HEADER         0
#define ZLIBWT_DECOMPR_STAGE_FILENAME       1
#define ZLIBWT_DECOMPR_STAGE_READFILE       2
#define ZLIBWT_DECOMPR_STAGE_FILE_GAP       3

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
    struct SDirIterFileData                 fileData;
    struct SZlibWtDecompressCallbacks       clbks;
    void*                                   userData;
    Bytef*                                  bufferForDecompressedDataTmp;

    size_t									stage;  // 0 - reading header, 1 - reading file name, 2 - reding file
    size_t									offsetInTheSection;
    size_t                                  sizeOfSection;

    size_t                                  reserved01;
    uInt                                    sizeForBufferForDecompressedDataTmp;
    uInt                                    isInited : 1;
    uInt                                    hasError : 1;
    uInt                                    reserved03 : 30;
};

CPPUTILS_END_C



#endif  // #ifndef ZLIB_WITH_TOOLS_SRC_INCLUDE_PRIVATE_ZLIB_WITH_TOOLS_ZLIB_DECOMPRESS_DATA_H
