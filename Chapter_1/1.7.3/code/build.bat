@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.7.3\code\breakdur.c
popd