@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.2.1\code\Size_Print.c
popd