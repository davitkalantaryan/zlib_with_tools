#   
# file:		zlibwt_hl_test.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("file:  zlibwt_hl_test.pro  ")

repositoryRoot = $${PWD}/../../..
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/contrib/cinternal/prj/common/common_qt/sys_common.pri)
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/contrib/cinternal/prj/common/common_qt/flags_common.pri)


QT -= core
QT -= gui
CONFIG -= qt

DEFINES += ZLIB_CONST

INCLUDEPATH += "$${PWD}/../../../include"
INCLUDEPATH += "$${PWD}/../../../contrib/zlib"
INCLUDEPATH += $${PWD}/../../../contrib/system/include
INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/include
INCLUDEPATH += $${PWD}/../../../contrib/qtutils/contrib/cpputils/contrib/cinternal/include
INCLUDEPATH += $${PWD}/../../../contrib/directory_iterator/include
INCLUDEPATH += $${PWD}/../../../src/include
INCLUDEPATH += $${PWD}/../../../contrib/system/src/include


LIBS += $${DESTDIR}/../lib/libzlib_st.a

SOURCES		+=	\
	\
        $${PWD}/../../../src/tools/self_extract/main_self_extract_common.cpp                        \
        $${PWD}/../../../src/core/zlibwt_ll_compression_routines.cpp                                \
        $${PWD}/../../../src/core/zlibwt_compression_routines.cpp                                   \
        $${PWD}/../../../src/core/zlibwt_decompress_routines.cpp                                    \
        $${PWD}/../../../src/core/zlibwt_decompress_routines_version_0000.cpp                       \
        $${PWD}/../../../src/core/zlibwt_ll_decompress_routines.cpp                                 \
        $${PWD}/../../../src/core/zlibwt_hl_decompress_routines.cpp                                 \
        $${PWD}/../../../src/core/resource_handler_unix.c                                           \
        $${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_unix.c      \
        $${PWD}/../../../contrib/system/src/core/system_create_process_unix.c

HEADERS += $$files($${repositoryRoot}/include/*.hpp,true)
HEADERS += $$files($${repositoryRoot}/include/*.h,true)
HEADERS += $$files($${repositoryRoot}/src/include/*.hpp,true)
HEADERS += $$files($${repositoryRoot}/src/include/*.h,true)

OTHER_FILES += $$files($${repositoryRoot}/src/core/*.cpp,true)
OTHER_FILES += $$files($${repositoryRoot}/src/core/*.c,true)
