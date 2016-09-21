#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
FLAGS="-O3 -Wall -Iinclude `pkg-config --cflags libcurl jansson libarchive`"
LIBS="`pkg-config --libs libcurl jansson libarchive`"
g++ $FLAGS \
	src/main.cpp\
	src/args.cpp\
	src/GetCurrentDir.cpp\
	src/updater_check.cpp\
	src/updater_download.cpp\
	src/update.cpp\
	$LIBS\
	-o Wicher
