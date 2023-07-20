//
// file:			resource_handler.h
// path:			include/resource_handler/resource_handler.h
// created on:		2023 Feb 19
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <zlib_with_tools/export_symbols.h>

#ifdef _WIN32

#include <resource_handler/resource_handler.h>
#include <zlib_with_tools/utils/stdio_zlibandtls.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cinternal/disable_compiler_warnings.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_BEGIN_C

// see: https://www.youtube.com/watch?v=aLTQvS7kn2A

#pragma pack(push,2)
typedef struct {
    WORD _id_reserved;
    WORD _id_type;
    WORD _id_count;
    BYTE _width;
    BYTE _height;
    BYTE _color_count;
    BYTE _reserved;
    WORD _planes;
    WORD _bit_count;
    DWORD _bytes_in_resource;
    DWORD _image_offset;
}ICON_ENTRY;
#pragma pack(pop)


ZLIBANDTLS_EXPORT int ResourceHandlerSecIconToApp(const char* a_path_to_app, const char* a_path_to_icon)
{
    ICON_ENTRY icon_entry;
    BOOL result;
    HANDLE hUpdateRes;
    char* pcBufferForIcon;
    struct stat fStat;
    FILE* fpIcoFile = fopen_zlibandtls(a_path_to_icon, "rb");
    if (!fpIcoFile) {
        //::std::cerr << "Unable to open \"" << a_path_to_icon << "\"file\n";
        return 1;
    }

    if (fstat(fileno_zlibandtls(fpIcoFile), &fStat)) {
        //::std::cerr << "Unable to stat \"" << a_path_to_icon << "\"file\n";
        fclose(fpIcoFile);
        return 1;
    }

    pcBufferForIcon = (char*)malloc(fStat.st_size);
    if (!pcBufferForIcon) {
        //::std::cerr << "Unable to allocate buffer\n";
        fclose(fpIcoFile);
        return 1;
    }

    fread(pcBufferForIcon, 1, fStat.st_size, fpIcoFile);
    fclose(fpIcoFile);


    hUpdateRes = BeginUpdateResourceA(a_path_to_app, FALSE);
    if (!hUpdateRes) {
        free(pcBufferForIcon);
        //::std::cerr << a_path_to_app << "is not a PE\n";
        return 1;
    }

    result = UpdateResource(
        hUpdateRes,                                 // update resource handle
        RT_ICON,                                    // change dialog box resource
        MAKEINTRESOURCE(1),                         // dialog box id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // neutral language
        pcBufferForIcon + 22,                       // ptr to resource info
        fStat.st_size - 22                          // size of resource info
    );
    if (!result) {
        EndUpdateResourceA(hUpdateRes, FALSE);
        free(pcBufferForIcon);
        //::std::cerr << "UpdateResource failed!\n";
        return 1;
    }


    // todo:
    icon_entry._id_reserved = 0;
    icon_entry._id_type = 1;
    icon_entry._id_count = 1;
    icon_entry._width = 32;
    icon_entry._height = 32;
    icon_entry._color_count = 0;
    icon_entry._reserved = 0;
    icon_entry._planes = 2;
    icon_entry._bit_count = 32;
    icon_entry._bytes_in_resource = fStat.st_size - 22;
    icon_entry._image_offset = 1;

    result = UpdateResource(
        hUpdateRes,                                 // update resource handle
        RT_GROUP_ICON,                              // change dialog box resource
        TEXT("MAINICON"),                           // dialog box id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // neutral language
        &icon_entry,                                // ptr to resource info
        sizeof(ICON_ENTRY)                          // size of resource info
    );
    EndUpdateResourceA(hUpdateRes, FALSE);
    free(pcBufferForIcon);
    if (!result) {
        //::std::cerr << "UpdateResource failed!\n";
        return 1;
    }
    return 0;
}


#endif  //  #ifdef _WIN32
