#
# repo:		zlib_with_tools
# file:		flagsandsys_common_pure.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#

!IFNDEF MakeFileDir
MakeFileDir			= $(MAKEDIR)\..
!ENDIF

!IFNDEF zlibWithToolsRepositoryRoot
zlibWithToolsRepositoryRoot	= $(MakeFileDir)\..\..\..
!ENDIF

!IFNDEF artifactRoot
artifactRoot	= $(zlibWithToolsRepositoryRoot)
!ENDIF

!IFNDEF directoryIteratorRepoRoot
directoryIteratorRepoRoot	= $(zlibWithToolsRepositoryRoot)\contrib\directory_iterator
!ENDIF

!IFNDEF qtutilsRepoRoot
qtutilsRepoRoot	= $(zlibWithToolsRepositoryRoot)\contrib\qtutils
!ENDIF

!IFNDEF systemRepositoryRoot
systemRepositoryRoot	= $(zlibWithToolsRepositoryRoot)\contrib\system
!ENDIF

!include <$(directoryIteratorRepoRoot)\prj\common\common_mkfl\flagsandsys_common.windows.Makefile>
!include <$(qtutilsRepoRoot)\prj\common\common_mkfl\flagsandsys_common.windows.Makefile>
!include <$(systemRepositoryRoot)\prj\common\common_mkfl\flagsandsys_common.windows.Makefile>

CFLAGS				= $(CFLAGS) /I"$(zlibWithToolsRepositoryRoot)\include"
CFLAGS				= $(CFLAGS) /I"$(zlibWithToolsRepositoryRoot)\contrib\zlib"

LFLAGS				= $(LFLAGS) /LIBPATH:"$(zlibWithToolsRepositoryRoot)\sys\win_$(Platform)\$(Configuration)\lib"
LFLAGS				= $(LFLAGS) /LIBPATH:"$(zlibWithToolsRepositoryRoot)\sys\win_$(Platform)\$(Configuration)\tlib"
