#   
# file:		zlibwt_hl_test.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  


message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )
DESTDIR     = "$${artifactRoot}/sys/$${CODENAME}/$$CONFIGURATION/test"


TARGET = main.exe

QT -= core
QT -= gui
CONFIG -= qt


SOURCES		+=	\
	\
        $${PWD}/../../../src/tests/main_hello_world_test.cpp
