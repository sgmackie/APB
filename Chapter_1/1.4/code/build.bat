@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1.4\code\Command_MIDI_Freq.c
popd