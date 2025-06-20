#define CURL_STATICLIB

#include <iostream>
#include <string>
#include "curl/curl.h"

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata) {

	int curlCount{};
	int eventCount{};
	int commitCount{};	

	std::string header = "";
	std::string value = "";

	bool isQuote = false;
	bool isValue = false;

	/*BAD CODE*/
	bool flag = false; //If false then there's no data
	bool isNameNamed = false;
	/*BAD CODE*/

	for (size_t i{0}; i < nmemb; i++) {

		if (((char*)data)[i] == '{') {
			if (curlCount == 0) {				
				curlCount++;
				eventCount++;				
			}
			else {
				curlCount++;				

				header.resize(0);
				isValue = false;
			}
		}
		else if (((char*)data)[i] == '}') {
			curlCount--;			
		}
		else if (((char*)data)[i] == '"') {
			if (!isQuote) {
				isQuote = true;
			}
			else {
				isQuote = false;

				if (isValue) {					

					if (header == "type") {
						std::cout << "The type of event is " << value << std::endl;
						flag = true;
					}
					else if (header == "name" && isNameNamed == false) {
						std::cout << "The commit was associated with the project called " << value;
						isNameNamed = true;
					}
					else if (header == "created_at") {
						std::cout << " in " << value << "\n\n";
						isNameNamed = false;						
					}
					else if (value == "Not Found") {
						std::cout << "The api didn't find this user" << std::endl;
						flag = true;
					}
					
					header.resize(0);
					value.resize(0);
					isValue = false;
				}
			}
		}
		else if (((char*)data)[i + 1] == '[') {			
			commitCount = 1;			
			header = std::to_string(commitCount);
		}
		else if (((char*)data)[i] == ':') {
			if (((char*)data)[i + 1] == '{') {
				isValue = false;
			}
			else if (((char*)data)[i + 1] != '"') {
				isQuote = true;
				isValue = true;
			}
			else {
				isValue = true;
			}
		}
		else if (((char*)data)[i] == ',') {
			if (isQuote) {
				isQuote = false;
				isValue = false;

				if (value == "Not Found") {
					std::cout << "The api didn't find this user" << std::endl;
					flag = true;
				}
				
				header.resize(0);
				value.resize(0);
			}
		}
		else {
			if (isQuote) {
				if (isValue) {
					value += ((char*)data)[i];					
				}
				else {
					header += ((char*)data)[i];							
				}
			}
		}
	}
	if (!flag) {
		std::cout << "There haven't been any events with this account" << std::endl;
	}

	return nmemb;
}

int main() {
	
	CURL* curl;
	CURLcode result;
	std::string nickName;
	std::cout << "github-activity (nickname) : ";
	std::cin >> nickName;
	std::string HTTPrequest = "https://api.github.com/users/" + nickName + "/events";

	curl = curl_easy_init();
	if (curl == NULL) {
		return -1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, HTTPrequest);
	curl_easy_setopt(curl, CURLOPT_USERAGENT);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
	curl_easy_setopt(curl, CURLOPT_USERAGENT);

	result = curl_easy_perform(curl);	
	if (result != CURLE_OK) {
		return -1;
	}	
	curl_easy_cleanup(curl);	
	return 0;
}