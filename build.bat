@echo off


set CFLAGS=-Wall -Wextra -Wno-address -std=c11 -ggdb

set src=..\src\violet.c
set name=violet

if not exist build mkdir build
pushd build
gcc %CFLAGS% -o %name% %src%
popd

