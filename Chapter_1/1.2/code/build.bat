@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.2\code\Freq_MIDI.c
popd