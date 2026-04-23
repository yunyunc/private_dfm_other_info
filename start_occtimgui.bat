@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "REPO_ROOT=%~dp0"
if "%REPO_ROOT:~-1%"=="\" set "REPO_ROOT=%REPO_ROOT:~0,-1%"

set "BUILD_DIR=%REPO_ROOT%\build\DebugWT"
set "CONFIG=Debug"
set "EXE_PATH=%BUILD_DIR%\bin\%CONFIG%\%CONFIG%\OcctImgui.exe"

set "RECONFIGURE=0"
set "REBUILD=0"
set "BUILD_ONLY=0"

:parse_args
if "%~1"=="" goto args_done
if /I "%~1"=="-reconfigure" set "RECONFIGURE=1" & shift & goto parse_args
if /I "%~1"=="-rebuild" set "REBUILD=1" & shift & goto parse_args
if /I "%~1"=="-buildonly" set "BUILD_ONLY=1" & shift & goto parse_args
if /I "%~1"=="-h" goto usage
if /I "%~1"=="--help" goto usage
echo Unknown argument: %~1
goto usage

:args_done
if "%RECONFIGURE%"=="1" goto do_configure
if not exist "%BUILD_DIR%\CMakeCache.txt" goto do_configure
goto after_configure

:do_configure
call :run cmake -S "%REPO_ROOT%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 exit /b 1

:after_configure
if "%REBUILD%"=="1" (
    if exist "%BUILD_DIR%" (
        call :run cmake --build "%BUILD_DIR%" --config %CONFIG% --target clean
        if errorlevel 1 exit /b 1
    )
)

if "%RECONFIGURE%"=="1" goto do_build
if "%REBUILD%"=="1" goto do_build
if not exist "%EXE_PATH%" goto do_build
goto after_build

:do_build
call :run cmake --build "%BUILD_DIR%" --config %CONFIG% --target OcctImgui
if errorlevel 1 exit /b 1

:after_build
call :prepend_if_exists "%BUILD_DIR%\vcpkg_installed\x64-windows\debug\bin"
call :prepend_if_exists "%BUILD_DIR%\vcpkg_installed\x64-windows\bin"
call :prepend_if_exists "%BUILD_DIR%\bin\%CONFIG%\%CONFIG%"
call :prepend_if_exists "%REPO_ROOT%\IFR\installed\standalone"

if "%BUILD_ONLY%"=="1" (
    echo Build completed. Exe: %EXE_PATH%
    exit /b 0
)

if not exist "%EXE_PATH%" (
    echo OcctImgui.exe not found: %EXE_PATH%
    exit /b 1
)

pushd "%BUILD_DIR%\bin\%CONFIG%\%CONFIG%"
echo Launching: %EXE_PATH%
"%EXE_PATH%"
set "EXIT_CODE=%ERRORLEVEL%"
popd
exit /b %EXIT_CODE%

:run
echo ^>^> %*
%*
exit /b %ERRORLEVEL%

:prepend_if_exists
if exist "%~1\" set "PATH=%~1;%PATH%"
exit /b 0

:usage
echo Usage: start_occtimgui.bat [-reconfigure] [-rebuild] [-buildonly]
exit /b 1
