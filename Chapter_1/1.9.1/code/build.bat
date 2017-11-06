@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.9.1\code\sine_text.c
popd