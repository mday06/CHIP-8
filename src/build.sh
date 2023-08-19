#!/bin/sh 

set -xe 

clang -Wall -Wextra -c chip8.c -o chip8.o
clang -Wall -Wextra -c graphics.c -o graphics.o `pkg-config --cflags sdl2`
clang -Wall -Wextra -c main.c -o main.o `pkg-config --cflags sdl2`
clang -Wall -Wextra chip8.o graphics.o main.o -o emulator `pkg-config --libs sdl2`
