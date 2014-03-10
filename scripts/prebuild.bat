@echo off

REM Must be run before compiling

set "scriptRoot=%~dp0"

REM ===================================================

REM Try to get the directory of the batch file
SET "hgRoot=%~dp0"
SET "hgRoot=%hgRoot:~0,-1%\.."

if not exist "%hgRoot%\.hg" GOTO WorkingDirError

REM ===================================================

set appName=%~1
IF "%~1"=="" set appName=SCME

REM Get the revision number
for /f "delims=" %%a in ('hg parent -R "%hgRoot%" --template "{rev}"') do @set revNum=%%a

REM Get the revision node hash
for /f "delims=" %%a in ('hg parent -R "%hgRoot%" --template "{node|short}"') do @set revChangeset=%%a

REM Get the branch
for /f "delims=" %%a in ('hg parent -R "%hgRoot%" --template "{branch}"') do @set revBranch=%%a

REM Get the major/minor versions
set major=0
set minor=0
set rev=0

if not exist "%hgRoot%\version" goto SkipVersion
(
set /p major=
set /p minor=
set /p rev=
)<"%hgRoot%\version"
:SkipVersion

REM put some info we gathered here in a file so that the post-build script can use it
(echo %appName%)> "%scriptRoot%buildinfo"
(echo %major%)>> "%scriptRoot%buildinfo"
(echo %minor%)>> "%scriptRoot%buildinfo
(echo %rev%)>> "%scriptRoot%buildinfo
(echo %revNum%)>> "%scriptRoot%buildinfo"
(echo %date%)>> "%scriptRoot%buildinfo"
(echo %revBranch%)>> "%scriptRoot%buildinfo"
(echo %revChangeset%)>> "%scriptRoot%buildinfo"

REM create a temporary appver.h file
set "appverPath=%hgRoot%\src\appver.h"

(echo #define APP_VERSION_MAJOR %major%)> "%appverPath%.tmp"
(echo #define APP_VERSION_MINOR %minor%)>> "%appverPath%.tmp"
(echo #define APP_VERSION_REV %rev%)>> "%appverPath%.tmp"
(echo #define APP_VERSION_BUILD %revNum%)>> "%appverPath%.tmp"
(echo #define APP_VERSION_CHANGESET "%revChangeset%")>> "%appverPath%.tmp"
(echo #define APP_NAME "%appName%")>> "%appverPath%.tmp"

REM Compare appver.h and appver.h.tmp
if not exist "%appverPath%" goto Different

FC /L "%appverPath%" "%appverPath%.tmp">nul
if errorlevel 1 goto Different
goto Same

:Different
REM reset errorlevel
ver>nul

move /Y "%appverPath%.tmp" "%appverPath%"
goto EndAppVer

:Same
REM get rid of appver.h.tmp
del "%appverPath%.tmp"

:EndAppVer





GOTO Success

:WorkingDirError
echo Invalid working directory
EXIT /B 1

GOTO Success

:Success

