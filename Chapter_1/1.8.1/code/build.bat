@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.8.1\code\exponential_decay.c
popd