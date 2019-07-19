#pragma once

int* loadGridFromFilename(char* filename, int totalWidth, int totalHeight);
int* loadGridFromFilename(std::string filename, int totalWidth, int totalHeight);
int* loadGridFromURL(char* url, int totalWidth, int totalHeight);
int* loadGridFromURL(std::string url, int totalWidth, int totalHeight);
int* loadGridFromString(char* data, int totalWidth, int totalHeight);
int* loadGridFromString(std::string data, int totalWidth, int totalHeight);
int* loadGridFromStream(std::istream& in, int totalWidth, int totalHeight);
