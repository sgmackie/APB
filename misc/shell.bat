@echo off

:: Set working directory path
set SourceDir=%~dp0
cd /d %SourceDir%..

:: Set Visual Studio debug mode
:: set VSCMD_DEBUG=1

:: Call Visual Studio 64bit compiler
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Set path for compiler to reference
set path=%SourceDir%;%path% 

set INCLUDE=%SourceDir%..\misc\include;%INCLUDE%

:: Set build and clean macros for terminal
@DOSKEY build=%SourceDir%\scripts\build_macro.bat
@DOSKEY clean=%SourceDir%\scripts\clean_macro.bat