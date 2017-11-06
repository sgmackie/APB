@echo off

set Debug=-FAsc -Zi

mkdir ..\build
pushd ..\build
cl %Debug% ..\..\2.3.3\code\soundfile_pan.cpp ..\..\2.3.3\code\soundfile_breakpoints.cpp ..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
popd




