#pragma once
#include "Weather.h"
#include "WeatherAPI.h"
#include <string>
#include <iomanip>
using namespace std;

class Favourite {
public:
	void saveFavoriteLocationToFile(const pair<string, pair<double, double>>& favoriteLocation);
	void viewFavourite();
	vector<pair<string, pair<double, double>>> readFavouriteFromFile();
};