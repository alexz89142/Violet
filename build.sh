#!/bin/sh

set -xe

mkdir -p bin
cd bin

CC=gcc
CFLAGS="-Wall -Wextra -Wno-address -std=c11 -ggdb"

src=../src/violet.c
name=violet

$CC $CFLAGS -o $name $src

cd ..
