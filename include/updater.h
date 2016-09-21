#ifndef UPDATER_H
#define UPDATER_H

#include <iostream>

#include <curl/curl.h>
#include <jansson.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#define WIN
#elif defined(__unix__)
#include <unistd.h>
#define UNI
#endif

#define UPDATE_URL "http://files.majudev.net/Wicher/versions/update.json"

bool check_for_update();
bool download_update();
void update();

#endif
