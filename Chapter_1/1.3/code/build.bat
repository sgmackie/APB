@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.3\code\Interactive_MIDI_Freq.c
popd