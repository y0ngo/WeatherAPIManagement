#pragma once
#ifndef LOCATION_MANAGEMENT_H
#define LOCATION_MANAGEMENT_H



#include <iostream>
#include <vector> // Add this line
using namespace std;

class LocationManagement {
public:
    void saveLocationToFile(std::string& locationName, std::pair<double, double>& coordinates);
    void removeLocationFromFile(const std::string& locationName);
    vector<pair<string, pair<double, double>>> readLocationsFromFile();
   

};

#endif