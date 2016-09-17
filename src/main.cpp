#include "main.h"

#ifdef WIN
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	std::vector<std::string> args = get_args(lpCmdLine);
#elif defined(UNI)
int main(int argc, char * argv[]){
	std::vector<std::string> args = get_args(argc, argv);
#endif
	bool silent = false;
	bool fakerun = false;
	unsigned int x = 0;
	while(x != args.size()){
		if(args[x] == std::string("-silent")){
			silent = true;
		}else if(args[x] == std::string("-fakerun")){
			fakerun = true;
		}
		++x;
	}

	char workdir[FILENAME_MAX];
	if(!GetCurrentDir(workdir, sizeof(workdir))){
		std::cout << "Cannot get executable path. Exitting..." << std::endl;
		return -1;
	}else if(ChangeCurrentDir(workdir)){
		std::cout << "Cannot set working directory: " << workdir << ". Exitting..." << std::endl;
		return -1;
	}

	if(!fakerun){
		if(silent){
			std::cout << "Performing silent run (errors only)..." << std::endl;
		}
#ifdef WIN
		FILE * f = popen(".\\bin\\Wicher.exe", "r");
#elif defined(UNI)
		FILE * f = popen("bin/Wicher", "r");
#endif
		if(!f){
			std::cout << "Failed to start Wicher." << std::endl;
			return -1;
		}
		char buffer[1024];
		while(fgets(buffer, 1024, f)){
			if(!silent){
				std::cout << buffer;
			}
		}
		pclose(f);
	}else{
		std::cout << "Wicher started with -fakerun arg. Quitting..." << std::endl;
	}
	return 0;
}
