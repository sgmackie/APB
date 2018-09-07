@echo off

:: Set build directory relative to current drive and path
set BuildDir=%~dp0..\build

:: Create path if it doesn't exist
if not exist %BuildDir% mkdir %BuildDir%

:: Move to build directory
pushd %BuildDir%

:: Set compiler arguments
set Files=..\..\..\Chapter_2\2.1.3\code\file_to_float.cpp ..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
set Libs=user32.lib
set ObjDir=.\obj\

:: Set compiler flags:
:: Visual Compiler
:: -Zi enable debugging info
:: -FC use full path in diagnostics
:: -Fo path to store Object files
:: Personal:
:: -DDEBUG toggles debug_Message macro for printing variables
set CompilerFlags=-Zi -FC -Fo%ObjDir% -DDEBUG

:: Create Object directory if it doesn't exist
if not exist %ObjDir% mkdir %ObjDir%

:: Run Visual Studio compiler
cl %CompilerFlags% %Files% %Libs%

:: Jump out of build directory
popd