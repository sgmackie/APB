@echo off

set Debug=-FAsc -Zi

mkdir ..\build
pushd ..\build
cl %Debug% ..\..\2.4.4\code\soundfile_envelope.cpp ..\..\2.4.4\code\soundfile_breakpoints.cpp ..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
popd




