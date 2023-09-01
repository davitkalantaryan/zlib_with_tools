#   
# file:		zlibwt_hl_test.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )


QT -= core
QT -= gui
CONFIG -= qt
CONFIG += console

DEFINES += ZLIB_CONST

LIBS += -lzlib

SOURCES		+=	\
	\
	$${PWD}/../../../src/tools/main_compressdir.cpp                                         \
	$${PWD}/../../../src/core/zlibwt_ll_compression_routines.c				\
	$${PWD}/../../../src/core/zlibwt_compression_routines.c                                 \
        $${PWD}/../../../src/utils/common_argument_parser.cpp                                   \
	$${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_unix.c	\
	$${PWD}/../../../contrib/directory_iterator/src/core/diriter/directory_iterator_win.c

HEADERS += $$files($${zlibWithToolsRepositoryRoot}/include/*.hpp,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/include/*.h,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/src/include/*.hpp,true)
HEADERS += $$files($${zlibWithToolsRepositoryRoot}/src/include/*.h,true)

OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/src/core/*.cpp,true)
OTHER_FILES += $$files($${zlibWithToolsRepositoryRoot}/src/core/*.c,true)
