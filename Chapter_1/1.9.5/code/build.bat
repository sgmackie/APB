@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.9.5\code\tuning_fork_audio.c
popd