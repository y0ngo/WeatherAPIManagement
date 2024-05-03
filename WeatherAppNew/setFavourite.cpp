#include "Weather.h"
#include "WeatherAPI.h"
#include "locationManagement.h"
#include "setFavourite.h"
#include <string>
#include <iomanip>
#include<fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
void Weather::setFavourite() {
    LocationManagement locationManager;
    Favourite favouriteManager;
    Weather weather;
    int choice;
    Api api;

    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  Favourites Management   \n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t  1) Choose Favourite       |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  2) View Favourite Weather Data        |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  3) Return to main menu    |\n";
        cout << "\t\t\t\t                            |\n";

        cin >> choice;
        switch (choice) {
        case 1: {
            vector<pair<string, pair<double, double>>> locations = locationManager.readLocationsFromFile();
            cout << "Choose a favorite location:" << endl;
            for (size_t i = 0; i < locations.size(); ++i) {
                cout << i + 1 << ". " << locations[i].first << endl;
            }
            int favoriteIndex;
            cin >> favoriteIndex;
            if (favoriteIndex >= 1 && favoriteIndex <= locations.size()) {
                // Set the selected location as the favorite
                pair<string, pair<double, double>> favoriteLocation = locations[favoriteIndex - 1];

                // Save the favorite location to a CSV file
                favouriteManager.saveFavoriteLocationToFile(favoriteLocation);

                cout << "Favorite location set successfully." << endl;
            }
            else {
                cout << "Invalid choice. Please try again." << endl;
            }
            break;
        }
        case 2: {
            vector<pair<string, pair<double, double>>> favorites = favouriteManager.readFavouriteFromFile();
            if (favorites.empty()) {
                cout << "No favorite locations found." << endl;
                break;
            }

            cout << "Choose a favorite location to view weather data:" << endl;
            for (size_t i = 0; i < favorites.size(); ++i) {
                cout << i + 1 << ". " << favorites[i].first << endl;
            }
            int favoriteIndex;
            cin >> favoriteIndex;
            if (favoriteIndex >= 1 && favoriteIndex <= favorites.size()) {
                pair<string, pair<double, double>> selectedFavorite = favorites[favoriteIndex - 1];
                double latitude = selectedFavorite.second.first;
                double longitude = selectedFavorite.second.second;

                // Fetch and display weather data using obtained coordinates
                api.fetchAndDisplayDailyWeatherData(latitude, longitude);

                cout << "Press any key to return to the main menu...";
                cin.ignore(); // Clear input buffer
                cin.get();    // Wait for user to press a key
            }
            else {
                cout << "Invalid choice. Please try again." << endl;
            }
            break;
        }
        case 3: // Return to the main menu
            weather.menu(); // Assuming weather is an instance of the Weather class
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    } while (true);
}

vector<pair<string, pair<double, double>>> Favourite::readFavouriteFromFile() {
    vector<pair<string, pair<double, double>>> locations;
    ifstream file("favorites.csv");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string locationName, latitudeStr, longitudeStr;
            getline(ss, locationName, ',');
            getline(ss, latitudeStr, ',');
            getline(ss, longitudeStr, ',');
            double latitude = stod(latitudeStr);
            double longitude = stod(longitudeStr);
            locations.push_back({ locationName, {latitude, longitude} });
        }
        file.close();
    }
    return locations;
}

void Favourite::saveFavoriteLocationToFile(const pair<string, pair<double, double>>& favoriteLocation) {
    ofstream file("favorites.csv", ios::app);
    if (file.is_open()) {
        file << favoriteLocation.first << "," << favoriteLocation.second.first << "," << favoriteLocation.second.second << "\n";
        file.close();
        cout << "Favorite location saved to file." << endl;
    }
    else {
        cerr << "Unable to open file for writing." << endl;
    }
}
