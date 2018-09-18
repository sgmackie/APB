@echo off

:: Set build directory relative to current drive and path
set BuildDir=%~dp0..\build

:: Create build path if it doesn't exist
if not exist %BuildDir% mkdir %BuildDir%

:: Move to build directory
pushd %BuildDir%

:: Set compiler arguments
set Files=..\code\win32_sinegen.cpp
set ObjDir=.\obj\

:: Set compiler flags:
:: -Zi enable debugging info
:: -FC use full path in diagnostics
:: -Fo path to store Object files
:: -DDEBUG to enable custom debug macros
set CompilerFlags=-Zi -FC -Fo%ObjDir% -DDEBUG

:: Set debug path for logging files
set DebugDir=%~dp0..\build\debug

:: Create Object and Debug directories if they don't exist
if not exist %ObjDir% mkdir %ObjDir%
if not exist %DebugDir% mkdir %DebugDir%

:: Run Visual Studio compiler
cl %CompilerFlags% %Files%

:: Jump out of build directory
popd