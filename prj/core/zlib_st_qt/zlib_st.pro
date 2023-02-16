#   
# file:		zlib_st.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  

TEMPLATE = lib

message("file:  zlib.pro  ")
include($${PWD}/../../common/common_qt/zlib.pri)
repositoryRoot = $${PWD}/../../..
include ($${repositoryRoot}/ENVIRONMENT)
include($${repositoryRoot}/contrib/qtutils/contrib/cpputils/prj/common/common_qt/sys_common.pri)

CONFIG -= dll
CONFIG += staticlib
