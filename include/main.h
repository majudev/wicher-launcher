#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#include <direct.h>
#include <mbstring.h>
//#define GetCurrentDir _getcwd
#define ChangeCurrentDir _chdir
#define WIN
#elif defined(__unix__)
#include <unistd.h>
//#define GetCurrentDir getcwd
#define ChangeCurrentDir chdir
#define UNI
#endif

#ifdef WIN
std::vector<std::string> get_args(LPTSTR lpCmdLine);
int GetCurrentDir(char * ownPth, size_t ownPth_size);
#elif defined(UNI)
std::vector<std::string> get_args(int argc, char* argv[]);
int GetCurrentDir(char * path, size_t path_size);
#endif

#endif
