@echo off

set Debug=-FAsc -Zi

mkdir ..\build
pushd ..\build
cl %Debug% ..\..\2.2.1\code\sound_file_gain.c ..\..\..\misc\psfmaster\portsf\portsf.c ..\..\..\misc\psfmaster\portsf\ieee80.c
popd