#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
FLAGS="$FLAGS -O3 -Wall -Iinclude `pkg-config --cflags jansson libarchive` `curl-config --cflags`"
LIBS="$LIBS -static `pkg-config --libs jansson libarchive bzip2 liblzma` -liconv `curl-config --static-libs`"
g++ $FLAGS \
	src/main.cpp\
	src/args.cpp\
	src/GetCurrentDir.cpp\
	src/updater_check.cpp\
	src/updater_download.cpp\
	src/update.cpp\
	$LIBS\
	-o Wicher
