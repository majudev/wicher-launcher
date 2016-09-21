#include "updater.h"

#include <fstream>
#include <sstream>

size_t write_buffer(void *ptr, size_t size, size_t nmemb, std::stringbuf * buffer) {
	return buffer->sputn((const char *) ptr, size * nmemb);
}

bool check_for_update(){
	std::stringbuf buffer;
	bool flag = false;
	curl_global_init(CURL_GLOBAL_ALL);
	
	CURL * curl = curl_easy_init();
	if(curl){
		std::cout << "Downloading manifest..." << std::endl;
		curl_easy_setopt(curl, CURLOPT_URL, UPDATE_URL);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, char * buffer, size_t length);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		CURLcode res = curl_easy_perform(curl);
		if(res != CURLE_OK){
			std::cout << "Error when running query: " << curl_easy_strerror(res) << std::endl;
		}else{
			json_error_t error;
			json_t * root = json_loads(buffer.str().c_str(), 0, &error);
			if(root){
				json_t * currentver = json_object_get(root, "current_version");
				const char * current_str = json_string_value(currentver);
				std::string current;
				if(current_str) current = current_str;
				std::ifstream t("version.txt");
				std::string version;
				if(t.is_open()){
					t.seekg(0, std::ios::end);
					version.reserve(t.tellg());
					t.seekg(0, std::ios::beg);
					version.assign(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
				}
				if(current != version){
					if(current.empty()){
						std::cout << "Couldn't get current version. Aborting." << std::endl;
					}else{
						if(version.empty()){
							std::cout << "Cannot get current version. Updating." << std::endl;
						}else{
							std::cout << "Currently installed version: " << version << std::endl;
							std::cout << "Currently up-to-date version: " << current << std::endl;
							std::cout << "Updating." << std::endl;
						}
						flag = true;
					}
				}else std::cout << "Program is up to date" << std::endl;
			}else{
				std::cout << "JSON error when parsing server's response: " << error.text << std::endl;
        			std::cout << "\tSource: " << error.source << std::endl;
        			std::cout << "\tLine: " << error.line << std::endl;
        			std::cout << "\tColumn: " << error.column << std::endl;
        			std::cout << "\tPosition [bytes]: " << error.position << std::endl;
			}
		}
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return flag;
}