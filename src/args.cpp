#include "main.h"

#ifdef WIN
std::vector<std::string> get_args(LPTSTR lpCmdLine) {
	if(lpCmdLine != NULL){
		std::vector<std::string> args;

		size_t size = wcstombs(NULL, GetCommandLineW(), 0);
		char* str = new char[size + 1];
		wcstombs( str, GetCommandLineW(), size + 1 );


		size_t i = 0;
		while(i < size){
			if(str[i] == ' '){
				str[i] = '\0';
				args.push_back(std::string(&str[i+1])); // Keep track of the first char in each word
			}
			++i;
		}

		return args;
	}
	return std::vector<std::string>();
}
#elif defined(UNI)
std::vector<std::string> get_args(int argc, char* argv[]){
	std::vector<std::string> args;
	int x = 0;
	while(x != argc){
		args.push_back(std::string(argv[x]));
		++x;
	}
	return args;
}
#endif
