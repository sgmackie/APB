@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.5.2\code\nscale.c
popd