
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

echo ----- Create directories for building and installing -----

if not exist "build\" mkdir build
cd build

if not exist "install\" mkdir install
cd install
set INSTALL_PATH=%CD%

cd ..\..\..

echo ----- Download zlib and libpng -----

if not exist "zlib\" (
  git clone https://github.com/winlibs/zlib.git
  cd zlib
  git checkout zlib-1.2.11
  cd ..
)

if not exist "libpng\" (
  git clone https://github.com/winlibs/libpng.git
  cd libpng
  git checkout libpng-1.6.34
  cd ..
)

echo ----- Building zlib -----

cd zlib

if not exist "build\" mkdir build
cd build

if exist "build_rc_genicam_api\" (
  cd build_rc_genicam_api\
) else (
  mkdir build_rc_genicam_api\
  cd build_rc_genicam_api\
  cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%INSTALL_PATH%" ..\..
)

nmake install
if %ERRORLEVEL% NEQ 0 exit /b 1

cd ..\..\..

echo ----- Building libpng -----

cd libpng

if not exist "build\" mkdir build
cd build

if exist "build_rc_genicam_api\" (
  cd build_rc_genicam_api\
) else (
  mkdir build_rc_genicam_api\
  cd build_rc_genicam_api\
  cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%INSTALL_PATH%" ..\..
)

nmake install
if %ERRORLEVEL% NEQ 0 exit /b 1

cd ..\..\..

echo ----- Build rc_genicam_api -----

cd rc_genicam_api\build

if exist "build_rc_genicam_api\" (
  cd build_rc_genicam_api\
) else (
  mkdir build_rc_genicam_api\
  cd build_rc_genicam_api\
  cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%INSTALL_PATH%" ..\..
)

nmake install
if %ERRORLEVEL% NEQ 0 exit /b 1

cd ..

echo ----- Extracting files for publication -----

for /F "tokens=* USEBACKQ" %%F in (`git describe`) do (set VERSION=%%F)

set TARGET=rc_genicam_api-%VERSION%-win64
if not exist "%TARGET%\" mkdir %TARGET%

xcopy /s %INSTALL_PATH%\bin %TARGET%\bin\
if not exist "%TARGET%\include\" mkdir %TARGET%\include
xcopy /s %INSTALL_PATH%\include\rc_genicam_api %TARGET%\include\
if not exist "%TARGET%\lib\" mkdir %TARGET%\lib
copy %INSTALL_PATH%\lib\GCBase*lib %TARGET%\lib\
copy %INSTALL_PATH%\lib\GenApi*lib %TARGET%\lib\
copy %INSTALL_PATH%\lib\rc_genicam_api.lib %TARGET%\lib\

