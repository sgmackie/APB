@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.8.2\code\exponential_break.c
popd