#
# repo:		qtutils
# file:		flagsandsys_common_pure.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#

!IFNDEF MakeFileDir
MakeFileDir			= $(MAKEDIR)\..
!ENDIF

!IFNDEF qtutilsRepoRoot
qtutilsRepoRoot	= $(MakeFileDir)\..\..\..
!ENDIF

!IFNDEF artifactRoot
artifactRoot	= $(qtutilsRepoRoot)
!ENDIF

!IFNDEF emsutilsRepoRoot
emsutilsRepoRoot	= $(qtutilsRepoRoot)\contrib\emsutils
!ENDIF

!include <$(emsutilsRepoRoot)\prj\common\common_mkfl\flagsandsys_common.windows.Makefile>
CFLAGS				= $(CFLAGS) /I"$(qtutilsRepoRoot)\include"
