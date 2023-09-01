#   
# file:		zlib.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  

TEMPLATE = lib

message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )
include($${PWD}/../../common/common_qt/zlib.pri)
include ($${zlibWithToolsRepositoryRoot}/ENVIRONMENT)

VERSION = $$ZLIB_VERSION
