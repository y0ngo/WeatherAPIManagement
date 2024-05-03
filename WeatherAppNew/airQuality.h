
#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"

class Air {
public:
	void fetchAndDisplayHourlyAirData(double latitude, double longitude);
	void fetchAndDisplayEuropeanAirData(double latitude, double longitude);

private:
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};
