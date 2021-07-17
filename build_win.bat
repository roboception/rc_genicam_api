
:: Build script for rc_sgm_producer under Windows

@echo off
setlocal enabledelayedexpansion

where nmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo This must be run in Visual Studio command prompt for x64
  exit /b 1
)

where git >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo You must download and install git from: git-scm.com/download/win
  exit /b 1
)

where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo You must download and install cmake from: https://cmake.org/download/
  exit /b 1
)

:: Create directories for building and installing

if not exist "build\" mkdir build
cd build

for /F "tokens=* USEBACKQ" %%F in (`git describe`) do (set VERSION=%%F)

set TARGET=rc_genicam_api-%VERSION%-win64
if not exist "%TARGET%\" mkdir %TARGET%
cd %TARGET%
set INSTALL_PATH=%CD%
cd ..

if exist "build_rc_genicam_api\" (
  cd build_rc_genicam_api\
) else (
  mkdir build_rc_genicam_api\
  cd build_rc_genicam_api\
  cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%INSTALL_PATH%" ..\..
)

nmake install

cd ..\..
