#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"

using namespace rapidjson;

class Api {
public:
    std::pair<double, double> fetchLocation(const std::string& location);
    void fetchAndDisplayDailyWeatherData(double latitude, double longitude);
    void fetchAndDisplaySomeHourlyWeatherData(double latitude, double longitude);
    void fetchAndDisplayAllHourlyWeatherData(double latitude, double longitude);
    void fetchAndDisplaySolarRadiationData(double latitude, double longitude);
    void fetchAndDisplayPressureData(double latitude, double longitude);
    void fetchAndDisplayModelData(double latitude, double longitude);
    void fetchAndDisplayAdditionalData(double latitude, double longitude);


private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};

#endif

