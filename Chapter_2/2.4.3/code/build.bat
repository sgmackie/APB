@echo off

set Debug=-FAsc -Zi

mkdir ..\build
pushd ..\build
cl %Debug% ..\..\2.4.3\code\envelope_extract.cpp ..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
popd