
@echo off

rem
rem file:			windows_prepare_repository.bat
rem path:			scripts/windows_prepare_repository.bat
rem created on:		2023 Feb 03
rem created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
rem

setlocal EnableDelayedExpansion

set scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "repositoryRoot=%cd%\"

git submodule sync --recursive
git submodule update --init --recursive

copy /Y prj\common\common_vs\zlib_srcs.vcxproj.filters prj\core\zlib_vs\zlib.vcxproj.filters
copy /Y prj\common\common_vs\zlib_srcs.vcxproj.filters prj\core\zlib_st_vs\zlib_st.vcxproj.filters
copy /Y prj\common\common_vs\zlib_srcs.vcxproj.filters prj\core\zlib_st_dl_vs\zlib_st_dl.vcxproj.filters

endlocal
