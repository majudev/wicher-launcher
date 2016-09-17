#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
FLAGS="-O3 -Wall -Iinclude"
LIBS="-static"
g++ $FLAGS \
	src/main.cpp\
	src/args.cpp\
	src/GetCurrentDir.cpp\
	$LIBS\
	-o Wicher
