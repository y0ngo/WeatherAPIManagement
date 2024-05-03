#pragma once

#ifndef HISTORICALAPI_H
#define HISTORICAL_H

#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"

using namespace rapidjson;

class hApi {
public:

 
    void fetchAndDisplayHistoricalDailyWeatherData(double latitude, double longitude, std::string start_date, std::string end_date);
    void fetchAndDisplaySomeHistoricalHourlyWeatherData(double latitude, double longitude, std::string start_date, std::string end_date);
    void fetchAndDisplayAllHistoricalWeatherData(double latitude, double longitude);
    void fetchAndDisplayHistoricalSolarRadiationData(double latitude, double longitude, std::string start_date, std::string end_date);
    void fetchAndDisplayAdditionalHistoricalData(double latitude, double longitude, std::string start_date, std::string end_date);
    void fetchAndDisplayHistoricalModelData(double latitude, double longitude, std::string start_date, std::string end_date);


private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};

#endif