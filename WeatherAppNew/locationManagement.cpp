#include "Weather.h"
#include "WeatherAPI.h"
#include "locationManagement.h"
#include <string>
#include <iomanip>
#include<fstream>
#include <sstream>
using namespace std;

void Weather::addLocation() {
    LocationManagement locationManager;
    Weather weather;
    int choice;
    Api api;

    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  Location Management   \n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t  1) Add Location           |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  2) Modify Location        |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  3) Remove Location        |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  4) Return to Main Menu    |\n";
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

            // Fetch and display weather data using obtained coordinates
          

            // Save location data to file
            locationManager.saveLocationToFile(locationName, coordinates);
            cout << "Press any key to return to the main menu...";
            cin.ignore(); // Clear input buffer
            cin.get();    // Wait for user to press a key
            break;
        }
              case 2: {
    std::string locationNameToModify;
    cout << "Enter the name of the location to modify: ";
    cin >> locationNameToModify;

    // Check if the location exists
    bool locationFound = false;
    vector<pair<string, pair<double, double>>> locations = locationManager.readLocationsFromFile();
    for (const auto& location : locations) {
        if (location.first == locationNameToModify) {
            locationFound = true;
            // Display current details
            cout << "Current details for " << locationNameToModify << ": Latitude = " << location.second.first << ", Longitude = " << location.second.second << endl;

            // Prompt user for new details
            std::string newLocationName;
            double newLatitude, newLongitude;
            cout << "Enter new location name: ";
            cin >> newLocationName;
            cout << "Enter new latitude: ";
            cin >> newLatitude;
            cout << "Enter new longitude: ";
            cin >> newLongitude;

            // Update location data in file
            locationManager.removeLocationFromFile(locationNameToModify);

            cout << "Location '" << locationNameToModify << "' modified successfully." << endl;
            break;
        }
    }

    if (!locationFound) {
        cout << "Location '" << locationNameToModify << "' not found." << endl;
    }

    cout << "Press any key to return to the main menu...";
    cin.ignore(); // Clear input buffer
    cin.get();    // Wait for user to press a key
    break;
}

        case 3: {
            std::string locationNameToRemove;
            cout << "Enter the name of the location to remove: ";
            cin >> locationNameToRemove;

            // Remove the specified location
            locationManager.removeLocationFromFile(locationNameToRemove);

            cout << "Press any key to return to the main menu...";
            cin.ignore(); // Clear input buffer
            cin.get();    // Wait for user to press a key
            break;
        }
        case 4: {
            weather.menu(); // Return to main menu
            return;
        }
        default: {
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
        }
    } while (true);
}

void LocationManagement::saveLocationToFile( std::string& locationName,  std::pair<double, double>& coordinates) {
    std::ofstream file("location.txt", std::ios::app); // Open file in append mode
    if (file.is_open()) {
        // Write location data to file
        file << locationName << "," << coordinates.first << "," << coordinates.second << "\n";
        file.close();
        std::cout << "Location data saved to file." << std::endl;
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}
vector<pair<string, pair<double, double>>> LocationManagement::readLocationsFromFile() {
    vector<pair<string, pair<double, double>>> locations;
    ifstream file("location.csv");
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
void LocationManagement::removeLocationFromFile(const std::string& locationName) {
    std::ifstream inFile("location.txt");
    std::ofstream outFile("temp.txt");

    if (!inFile || !outFile) {
        std::cerr << "Error: Unable to open files." << std::endl;
        return;
    }

    std::string line;

    // Copy all lines except the one to be removed to temp.txt
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string currentLocationName;
        if (std::getline(iss, currentLocationName, ',')) {
            if (currentLocationName != locationName) {
                outFile << line << std::endl;
            }
        }
    }

    inFile.close();
    outFile.close();

    // Remove the original file
    if (std::remove("location.txt") != 0) {
        std::cerr << "Error: Unable to remove file." << std::endl;
        return;
    }

    // Rename temp.txt to location.txt
    if (std::rename("temp.txt", "location.txt") != 0) {
        std::cerr << "Error: Unable to rename file." << std::endl;
        return;
    }

    std::cout << "Location '" << locationName << "' removed successfully." << std::endl;
}
