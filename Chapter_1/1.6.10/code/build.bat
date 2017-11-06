@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.6.10\code\iscale.c
popd