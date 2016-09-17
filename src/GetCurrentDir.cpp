#include "main.h"

#ifdef WIN
int GetCurrentDir(char * ownPth, size_t ownPth_size){
	// Will contain exe path
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL){
		// When passing NULL to GetModuleHandle, it returns handle of exe itself
		GetModuleFileName(hModule,ownPth, ownPth_size);
		int x = strlen(ownPth);
		while(x != 0){
			if(ownPth[x] == '\\'){
				ownPth[x] = '\0';
				break;
			}
			--x;
		}
		return 1;
	}else{
		return 0;
	}
}
#elif defined(UNI)
int GetCurrentDir(char * path, size_t path_size){
	char tmp[256];
	sprintf(tmp, "/proc/%d/exe", getpid());
	ssize_t path_len = path_size - 1;
	int bytes = std::min(readlink(tmp, path, path_size), path_len);
	if(bytes > 0){
		path[bytes] = '\0';
		if(realpath(path, tmp)){
			strcpy(path, tmp);
		}

		int x = strlen(path);
                while(x != 0){
                        if(path[x] == '/'){
                                path[x] = '\0';
                                break;
                        }
                        --x;
                }

		return bytes;
	}
	return 0;
}
#endif
