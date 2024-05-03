#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "WeatherAPI.h"
#include "locationManagement.h"
#include "Weather.h"

using namespace std;

void Weather::viewLocation() {
    LocationManagement locationManager;
    Api api;
    int choice;
    Weather weather;

    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  View Any Location Data       \n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t  1) Enter Location/Weather Data        |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  2) Return to Main Menu    |\n";
        cout << "\t\t\t\t                            |\n";
        cin >> choice;

        switch (choice) {
        case 1: {
            std::string locationName;
            cout << "Enter location: ";
            cin >> locationName;

            // Fetch coordinates for the given location
            auto coordinates = api.fetchLocation(locationName);

            if (coordinates.first == 0.0 && coordinates.second == 0.0) {
                cout << "Location not found. Please try again." << endl;
                break;
            }

            // Display the coordinates
            cout << "Coordinates for " << locationName << ": Latitude = " << coordinates.first << ", Longitude = " << coordinates.second << endl;

            int weatherChoice;
            cout << "Which form of weather data would you like to view:\n";
            cout << "\t1) Some Hourly Weather Variables\n";
            cout << "\t2) All Hourly Weather Variables\n";
            cout << "\t3) Daily Weather Variables\n";
            cout << "\t4) Solar Radiation Variables\n";
            cout << "\t5) Pressure Variables\n";
            cout << "\t6) Model Variables\n";
            cout << "\t7) Return to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> weatherChoice;

            switch (weatherChoice) {
            case 1:
                api.fetchAndDisplaySomeHourlyWeatherData(coordinates.first, coordinates.second);
                break;
            case 2:
                api.fetchAndDisplayAllHourlyWeatherData(coordinates.first, coordinates.second);
                break;
            case 3:
                api.fetchAndDisplayDailyWeatherData(coordinates.first, coordinates.second);
                break;
            case 4:
                api.fetchAndDisplaySolarRadiationData(coordinates.first, coordinates.second);
                break;
            case 5:
                api.fetchAndDisplayPressureData(coordinates.first, coordinates.second);
                break;
            case 6:
                api.fetchAndDisplayModelData(coordinates.first, coordinates.second);
                break;
            case 7:
                // Return to the main menu
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
            }

            // Prompt the user to continue or return to the main menu
            cout << "Press 1 to continue or 2 to return to the main menu: ";
            cin >> choice;
            break;
        }
        case 2:
            // Return to the main menu
            weather.menu();
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    } while (choice != 2); // Continue looping until the user chooses to return to the main menu
}



