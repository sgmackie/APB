@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.1\code\Hello_Wonder.c
popd