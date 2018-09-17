@echo off

:: Set build directory relative to current drive and path
set BuildDir=%~dp0..\build

:: Create path if it doesn't exist
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

:: Create Object directory if it doesn't exist
if not exist %ObjDir% mkdir %ObjDir%

:: Run Visual Studio compiler
cl %CompilerFlags% %Files%

:: Jump out of build directory
popd