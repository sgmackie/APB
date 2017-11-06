@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.9.3\code\tuning_fork.c
popd