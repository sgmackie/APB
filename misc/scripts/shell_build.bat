@echo off

pushd misc
call shell.bat
popd

pushd Chapter_2\2.6.3\code
call build
popd

cd Chapter_2\2.6.3\build