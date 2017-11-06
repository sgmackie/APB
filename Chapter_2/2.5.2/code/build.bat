@echo off

:: Set build directory relative to current drive and path
set BuildDir=%~dp0..\build

:: Create path if it doesn't exist
if not exist %BuildDir% mkdir %BuildDir%

:: Move to build directory
pushd %BuildDir%

:: Set compiler arguments
set Files=..\code\synthesis_test.cpp ..\code\synthesis_wave.cpp ..\code\soundfile_breakpoints.cpp
set Includes=..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
set ObjDir=.\obj\

:: Set compiler flags:
:: -Zi enable debugging info
:: -FC use full path in diagnostics
:: -Fo path to store Object files
set CompilerFlags=-Zi -FC -Fo%ObjDir%

:: Create Object directory if it doesn't exist
if not exist %ObjDir% mkdir %ObjDir%

:: Run Visual Studio compiler
cl %CompilerFlags% %Files% %Includes%

:: Jump out of build directory
popd