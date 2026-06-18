::
:: repo:	    zlib_with_tools
:: file:	    windows_source_per_session.bat
:: path:	    scripts\windows_source_per_session.bat
:: created on:	2026 Jun 18
:: created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
:: notice:	    call this to initialize session for development
::

@echo off
setlocal EnableDelayedExpansion enableextensions

set  scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "zlibWithToolsRepositoryRoot=%cd%"
set "repositoryRoot=%zlibWithToolsRepositoryRoot%\"


endlocal & (
    call "%zlibWithToolsRepositoryRoot%\contrib\directory_iterator\scripts\windows_source_per_session.bat"
    call "%zlibWithToolsRepositoryRoot%\contrib\qtutils\scripts\windows_source_per_session.bat"
    call "%zlibWithToolsRepositoryRoot%\contrib\system\scripts\windows_source_per_session.bat"
    set "zlibWithToolsRepositoryRoot=%zlibWithToolsRepositoryRoot%"
)
