@echo off

:: Set directories to read into
set BuildDir=..\build
set ObjDir=.\obj\
set Debug=.\Debug\

:: Delete files if build directory exists
if exist %BuildDir% (  
  :: Move to build path
  pushd %BuildDir%

  :: Force delete files with the following extentions, without printing output to the terminal
  del /q /f *.exe *.pdb *.ilk *.dll
  
  :: Remove directory and sub-folders without printing output to the terminal
  if exist %ObjDir% (
    rd /q /s %ObjDir%
  )

  if exist %Debug% (
    rd /q /s %Debug%
  )
  
  :: Jump out of build directory
  popd
)