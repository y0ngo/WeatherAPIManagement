#pragma once
#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "exportDataChart.h"
#include <string>
#include "Weather.h"
#include "locationManagement.h"
#include "WeatherAPI.h"


void Export::exportCSV(double latitude, double longitude) {
    // Construct the CSV export URL
    std::string csvUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(latitude) +
        "&longitude=" + std::to_string(longitude) +
        "&hourly=temperature_2m,relative_humidity_2m,precipitation,pressure_msl,surface_pressure,cloud_cover&format=csv&timezone=GMT";

    // Open the CSV export URL in Chrome on Windows
    std::string openCommand = "start chrome \"" + csvUrl + "\"";
    system(openCommand.c_str());
}
void Export::exportJSON(double latitude, double longitude) {
    // Construct the JSON export URL
    std::string latitudeStr = std::to_string(latitude);
    std::string longitudeStr = std::to_string(longitude);
    std::string jsonUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + latitudeStr +
        "&longitude=" + longitudeStr +
        "&hourly=temperature_2m,relative_humidity_2m,precipitation_probability,surface_pressure,cloud_cover,wind_speed_10m";

    // Open the JSON export URL in Google Chrome
    std::string openCommand = "start chrome \"" + jsonUrl + "\""; // For Windows
    // std::string openCommand = "google-chrome \"" + jsonUrl + "\""; // For Linux
    system(openCommand.c_str());
}
void Export::exportXLSXE(double latitude, double longitude) {
    // Construct the XLSX export URL
    std::string latitudeStr = std::to_string(latitude);
    std::string longitudeStr = std::to_string(longitude);
    std::string xlsxUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + latitudeStr +
        "&longitude=" + longitudeStr +
        "&hourly=temperature_2m,relative_humidity_2m,precipitation_probability,surface_pressure,cloud_cover,wind_speed_10m&format=xlsx";

    // Open the XLSX export URL in the default web browser
    std::string openCommand = "start chrome \"" + xlsxUrl + "\""; // For Windows
    // std::string openCommand = "xdg-open \"" + xlsxUrl + "\""; // For Linux
    system(openCommand.c_str());
}
void Weather::exportDataChart() {
    Export exporter;
    Api api;
    int choice;

    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  Choose Export Option       \n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t  1) Export Hourly Data as CSV  |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  2) Export Hourly Data as JSON |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  3) Export Hourly Data as XLSX |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  4) Return to Main Menu    |\n";
        cout << "\t\t\t\t                            |\n";
        cin >> choice;

        switch (choice) {
        case 1:
        case 2:
        case 3: {
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

            // Export the data based on the choice
            if (choice == 1)
                exporter.exportCSV(coordinates.first, coordinates.second);
            else if (choice == 2)
                exporter.exportJSON(coordinates.first, coordinates.second);
            else if (choice == 3)
                exporter.exportXLSXE(coordinates.first, coordinates.second);

            break;
        }
        case 4:
            return; // Return to the main menu
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }

        // Prompt the user to continue or return to the main menu
        cout << "Press 1 to continue or 2 to return to the main menu: ";
        cin >> choice;
    } while (choice != 2); // Continue looping until the user chooses to return to the main menu
}

