#
# repo:		zlib_with_tools
# file:		flagsandsys_common.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#

!IFNDEF zlibWithToolsFlagsAndSysCommonIncluded
zlibWithToolsFlagsAndSysCommonIncluded		= 1
!IFNDEF zlibWithToolsRepositoryRoot
zlibWithToolsRepositoryRoot	= $(MAKEDIR)\..\..\..
!ENDIF
!include <$(zlibWithToolsRepositoryRoot)\prj\common\common_mkfl\raw\flagsandsys_common_raw.windows.Makefile>
!ENDIF
