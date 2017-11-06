@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.5.1\code\Loop_Test.c
popd