#include "updater.h"

void update(){
	if(check_for_update()){
		if(!download_update()){
			std::cout << "Couldn't update. Try do to it manually." << std::endl;
			std::cout << "More info at http://majudev.net/" << std::endl;
		}
	}
}
