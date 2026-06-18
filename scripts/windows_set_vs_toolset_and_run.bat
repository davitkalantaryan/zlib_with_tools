
@echo off

rem
rem	repository:		system
rem file:			windows_set_vs_toolset_and_run.bat
rem path:			scripts/windows_set_vs_toolset_and_run.bat
rem created on:		2023 May 18
rem created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
rem

setlocal EnableDelayedExpansion

set scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "repositoryRoot=%cd%\"

echo "repositoryRoot is %repositoryRoot%"

call "%scriptDirectory%windows_source_per_session.bat"
echo PlatformToolsetVar=%PlatformToolsetVar%


devenv workspaces\zlib_with_tools_all_vs\zlib_with_tools_all.sln

endlocal
