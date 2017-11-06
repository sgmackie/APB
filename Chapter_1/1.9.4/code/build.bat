@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.9.4\code\tuning_fork_2.c
popd