//
// file:			main_hello_world_test.cpp
// path:			src/tests/main_hello_world_test.cpp
// created on:		2023 Feb 17
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <zlib_with_tools/export_symbols.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <zlib_with_tools/utils/memory_zlibandtls.h>
#include <resource_handler/resource_handler.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cinternal/undisable_compiler_warnings.h>

#define ZLIBWT_EXE_NM_TO_SET_ICON               "hello_world_test.exe"
#define ZLIBWT_EXE_NM_TO_SET_ICON_LEN_PLUS1     21


int main(int a_argc, char* a_argv[])
{

    // icon = ExtractIconA(0, a_argv[1], 0);
    size_t unBeginSizePlus1;
    char* pcBuffer;
    char* pcTerm = strrchr(a_argv[0], '\\');
    assert(pcTerm);

    if (a_argc < 2) {
        fprintf(stderr,"icon file is not specified\n");
        return 1;
    }

    unBeginSizePlus1 = ((size_t)(pcTerm - a_argv[0]))+1;
    pcBuffer = (char*)alloca_zlibandtls(unBeginSizePlus1 + 4 + ZLIBWT_EXE_NM_TO_SET_ICON_LEN_PLUS1);
    memcpy(pcBuffer, a_argv[0], unBeginSizePlus1);
    memcpy(pcBuffer + unBeginSizePlus1, ZLIBWT_EXE_NM_TO_SET_ICON, ZLIBWT_EXE_NM_TO_SET_ICON_LEN_PLUS1);


    return ResourceHandlerSecIconToApp(pcBuffer,a_argv[1]);
}
