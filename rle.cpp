#include <iostream>
#include <fstream>
#include <sstream>
#include <curl\curl.h>
#include <string>
#include "rle.h"

int* loadGridFromFilename(char* filename, int totalWidth, int totalHeight) {
	std::ifstream file(filename);
	int* grid = loadGridFromStream(file, totalWidth, totalHeight);
	file.close();

	return grid;
}

int* loadGridFromFilename(std::string filename, int totalWidth, int totalHeight) {
	std::ifstream file(filename);
	int* grid = loadGridFromStream(file, totalWidth, totalHeight);
	file.close();

	return grid;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* stream)
{
	std::string data((const char*)contents, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data;
	return size * nmemb;
}

int* loadGridFromURL(char* url, int totalWidth, int totalHeight) {
	CURL* curl;
	std::stringstream ss;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ss);
	curl_easy_cleanup(curl);

	return loadGridFromStream(ss, totalWidth, totalHeight);
}

int* loadGridFromURL(std::string url, int totalWidth, int totalHeight) {
	CURL* curl;
	std::stringstream ss;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ss);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return loadGridFromStream(ss, totalWidth, totalHeight);
}

int* loadGridFromString(char* data, int totalWidth, int totalHeight) {
	std::istringstream in(data);
	int* grid = loadGridFromStream(in, totalWidth, totalHeight);

	return grid;
}

int* loadGridFromString(std::string data, int totalWidth, int totalHeight) {
	std::istringstream in(data);
	int* grid = loadGridFromStream(in, totalWidth, totalHeight);

	return grid;
}

int* loadGridFromStream(std::istream& in, int totalWidth, int totalHeight) {
	int* grid = new int[(long long) totalWidth * totalHeight];
	int startX = 0,
		startY = 0;
	std::memset(grid, 0, (long long) totalWidth * totalHeight);
	std::string line;
	while (std::getline(in, line)) {
		if (line[0] == '#') {
			continue;
		}
		int fileWidth = std::stoi(line.substr(line.find("x = ") + 4, line.find(", y")));
		int fileHeight = std::stoi(line.substr(line.find("y = ") + 4, line.find(", rule")));
		startX = (totalWidth - fileWidth) / 2;
		startY = (totalHeight - fileHeight) / 2;
		if (startX < 0) {
			std::cout << "Window width is too small for this file (min: " << fileWidth
				<< "). Use the -w or --width command line argument to set an appropriate width.";
		}
		if (startY < 0) {
			std::cout << "Window height is too small for this file (min: " << fileHeight
				<< "). Use the -h or --height command line argument to set an appropriate height.";
		}
		if (startX < 0 || startY < 0)
			return NULL;
		break;
	}
	int y = startY;
	int x = startX;
	char chr;
	std::string numBuffer;

	in >> std::skipws;

	while (in >> chr) {
		if (chr == '!')
			break;
		else if (chr == '$') {
			int len = numBuffer.length() == 0 ? 1 : std::stoi(numBuffer);
			y += len;
			x = startX;
			numBuffer.clear();
		}
		else if (chr == 'o') {
			int len = numBuffer.length() == 0 ? 1 : std::stoi(numBuffer);
			std::fill_n(grid + x + (long long) y * totalWidth, len, 1);
			x += len;
			numBuffer.clear();
		}
		else if (chr == 'b') {
			int len = numBuffer.length() == 0 ? 1 : std::stoi(numBuffer);
			x += len;
			numBuffer.clear();
		}
		else {
			numBuffer.push_back(chr);
		}
	}
	return grid;
}