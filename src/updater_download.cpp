#include "updater.h"

#include <cstdio>
#include <sstream>
#include <cmath>

#include <archive.h>
#include <archive_entry.h>

static int copy_data(struct archive *ar, struct archive *aw){
	int r;
	const void *buff;
	size_t size;
#ifdef WIN
	la_int64_t offset;
#else
	off_t offset;
#endif

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "%s\n", archive_error_string(aw));
			return (r);
		}
	}
}

static bool extract(char * buffer, size_t length){
	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int flags;
	int r;

	/* Select which attributes we want to restore. */
	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;

	a = archive_read_new();
	//archive_read_support_format_all(a);
	archive_read_support_format_tar(a);
	//archive_read_support_filter_all(a);
	archive_read_support_filter_xz(a);
	archive_read_support_filter_gzip(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);
	if ((r = archive_read_open_memory(a, buffer, length)))
		return false;
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(a));
		if (r < ARCHIVE_WARN)
			return false;
		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
		else if (archive_entry_size(entry) > 0) {
			r = copy_data(a, ext);
			if (r < ARCHIVE_OK)
				fprintf(stderr, "%s\n", archive_error_string(ext));
			if (r < ARCHIVE_WARN)
				return false;
		}
		r = archive_write_finish_entry(ext);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
		if (r < ARCHIVE_WARN)
			return false;
	}
	archive_read_close(a);
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);
	return true;
}

int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded){
	// ensure that the file to be downloaded is not empty
	// because that would cause a division by zero error later on
	if (TotalToDownload <= 0.0) {
		return 0;
	}

	// how wide you want the progress meter to be
	int totaldotz = 40;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	// part of the progressmeter that's already "full"
	int dotz = std::round(fractiondownloaded * totaldotz);

	// create the "meter"
	int ii=0;
	printf("%3.0f%% [",fractiondownloaded*100);
	// part  that's full already
	for ( ; ii < dotz;ii++) {
		printf("=");
	}
	// remaining part (spaces)
	for ( ; ii < totaldotz;ii++) {
		printf(" ");
	}
	// and back to line begin - do not forget the fflush to avoid output buffering problems!
	printf("]\r");
	fflush(stdout);
	// if you don't return 0, the transfer will be aborted - see the documentation
	return 0; 
}

size_t write_data(void *ptr, size_t size, size_t nmemb, std::stringbuf * buffer) {
	return buffer->sputn((const char *) ptr, size * nmemb);
}

bool download_update(){
	std::stringbuf buffer;
	bool flag = false;
	curl_global_init(CURL_GLOBAL_ALL);
	
	CURL * curl = curl_easy_init();
	if(curl){
		std::cout << "Downloading manifest..." << std::endl;
		curl_easy_setopt(curl, CURLOPT_URL, UPDATE_URL);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, char * buffer, size_t length);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		CURLcode res = curl_easy_perform(curl);
		if(res != CURLE_OK){
			std::cout << "Error when running query: " << curl_easy_strerror(res) << std::endl;
		}else{
			json_error_t error;
			json_t * root = json_loads(buffer.str().c_str(), 0, &error);
			if(root){
				json_t * downloadlink_obj = json_object_get(root, "download_link");
				const char * downloadlink_str = json_string_value(downloadlink_obj);
				if(downloadlink_str){
					std::cout << "Downloading package..." << std::endl;
					std::string downloadlink = downloadlink_str;
					std::stringbuf data;
					curl_easy_setopt(curl, CURLOPT_URL, downloadlink_str);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
					curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
					curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
					res = curl_easy_perform(curl);
					std::cout << std::endl;
					if(res != CURLE_OK){
						std::cout << "Error when downloading. Aborting." << std::endl;
					}else{
						std::cout << "Extracting package..." << std::endl;
						if(extract((char*)data.str().c_str(), data.str().size())){
							std::cout << "OK." << std::endl;
							flag = true;
						}else{
							std::cout << "Error when unpacking. Aborting. You may have to reinstall the program." << std::endl;
						}
					}
				}else std::cout << "Cannot get download link. Aborting." << std::endl;
			}else{
				std::cout << "JSON error parsing manifest: " << error.text << std::endl;
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
