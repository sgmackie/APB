@echo off

mkdir ..\build
pushd ..\build
cl -FAsc -Zi ..\..\1_Arthimatic_Test\code\1_Arthimatic_Test_SRC.c
popd