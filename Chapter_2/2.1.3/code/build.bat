@echo off

set Debug=-FAsc -Zi

mkdir ..\build
pushd ..\build
cl %Debug% ..\..\2.1.3\code\file_to_float.c ..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
popd