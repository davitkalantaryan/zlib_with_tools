#   
# file:		zlibwt_hl_test.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("file:  zlibwt_hl_test.pro  ")

repositoryRoot = $${PWD}/../../..
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/contrib/cinternal/prj/common/common_qt/sys_common.pri)
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/contrib/cinternal/prj/common/common_qt/flags_common.pri)

DESTDIR     = $${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test
message(DESTDIR is $$DESTDIR)

TARGET = main.exe

QT -= core
QT -= gui
CONFIG -= qt


SOURCES		+=	\
	\
        $${PWD}/../../../src/tests/main_hello_world_test.cpp
