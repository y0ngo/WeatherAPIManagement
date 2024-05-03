#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "airQuality.h"
#include "WeatherAPI.h"
#include "Weather.h"
#include "locationManagement.h"

using namespace rapidjson;
using namespace std;

size_t Air::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response_data) {
    size_t total_size = size * nmemb;
    response_data->append((char*)contents, total_size);
    return total_size;
}
void Air::fetchAndDisplayHourlyAirData(double latitude, double longitude) {
    CURL* curl = curl_easy_init();

    if (curl) {
        std::string unit_choice;

     
    
        std::string time_format_param;
        std::cout << "Choose time format (ISO,unixtime): " << std::endl;
        std::cin >> unit_choice;
        if (unit_choice == "unixtime") {
            time_format_param = "&timeformat=" + unit_choice;
        }
        else if (unit_choice != "ISO") {
            std::cerr << "Invalid time format.Defaulting to ISO." << std::endl;
        }
        std::string timezone;
        std::cout << "Enter timezone (e.g., GMT, EST): ";
        std::cin >> timezone;
           std::string past_days_format;
       std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
       std::cin >> unit_choice;
       if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
           past_days_format = " &past_days=" + unit_choice;
       }
       else if (unit_choice != "0") {
           std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
       }
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }

        std::string domain_param;
        std::cout << "Enter Domain (cams,cams_global,cams_europe): " << std::endl;
        std::cin >> unit_choice;
        if (unit_choice == "cams_global" || unit_choice == "cams_europe") {
            domain_param = "&domains=" + unit_choice;
        }
        else if (unit_choice != "cams") {
            std::cerr << "Invalid domain format.defaulting to cams" << std::endl;
        }

        std::string url = "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=" + std::to_string(latitude) +
            "&longitude=" + std::to_string(longitude) +
            "&hourly=pm10,pm2_5,carbon_monoxide,nitrogen_dioxide,sulphur_dioxide,ozone,aerosol_optical_depth,dust,uv_index,uv_index_clear_sky,ammonia,alder_pollen,birch_pollen,grass_pollen,mugwort_pollen,olive_pollen,ragweed_pollen"+time_format_param+"&timezone="+timezone + forecast_days_format + domain_param;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            long http_response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
            std::cout << "HTTP Response Code: " << http_response_code << std::endl;
           // std::cout << "Response Data: " << response_data << std::endl; // Print response data for debugging
        }

        Document parsedData;
        parsedData.Parse(response_data.c_str());

        if (parsedData.HasParseError()) {
            std::cerr << "Error parsing JSON." << std::endl;
        }
        else {
            double latitude = parsedData["latitude"].GetDouble();
            double longitude = parsedData["longitude"].GetDouble();

            const Value& hourlyData = parsedData["hourly"];

            // Extract the required air quality data
            const Value& pm10 = hourlyData["pm10"];
            const Value& pm25 = hourlyData["pm2_5"];
            const Value& carbon_monoxide = hourlyData["carbon_monoxide"];
            const Value& nitrogen_dioxide = hourlyData["nitrogen_dioxide"];
            const Value& sulphur_dioxide = hourlyData["sulphur_dioxide"];
            const Value& ozone = hourlyData["ozone"];
            const Value& aerosol_optical_depth = hourlyData["aerosol_optical_depth"];
            const Value& dust = hourlyData["dust"];
            const Value& uv_index = hourlyData["uv_index"];
            const Value& uv_index_clear_sky = hourlyData["uv_index_clear_sky"];
           /* const Value& ammonia = hourlyData["ammonia"];*/
            // Add more variables as needed

            // Extract the time data
            const Value& hourlyTime = hourlyData["time"];

            // Output the air quality data
            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Air Quality Data:" << std::endl;
            for (SizeType i = 0; i < pm10.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl; // Print time
                std::cout << "PM10: " << pm10[i].GetDouble() << std::endl;
                std::cout << "PM2.5: " << pm25[i].GetDouble() << std::endl;
                std::cout << "Carbon Monoxide: " << carbon_monoxide[i].GetDouble() << std::endl;
                std::cout << "Nitrogen Dioxide: " << nitrogen_dioxide[i].GetDouble() << std::endl;
                std::cout << "Sulphur Dioxide: " << sulphur_dioxide[i].GetDouble() << std::endl;
                std::cout << "Ozone: " << ozone[i].GetDouble() << std::endl;
                std::cout << "Aerosol Optical Depth: " << aerosol_optical_depth[i].GetDouble() << std::endl;
                std::cout << "Dust: " << dust[i].GetDouble() << std::endl;
                std::cout << "UV Index: " << uv_index[i].GetDouble() << std::endl;
                std::cout << "UV Index Clear Sky: " << uv_index_clear_sky[i].GetDouble() << std::endl;
                /*std::cout << "Ammonia: " << ammonia[i].GetDouble() << std::endl;*/
                // Add more output variables as needed

                // Add a gap after each hourly data set
                std::cout << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}
void Air::fetchAndDisplayEuropeanAirData(double latitude, double longitude) {
    CURL* curl = curl_easy_init();

    if (curl) {
        std::string unit_choice;



        std::string time_format_param;
        std::cout << "Choose time format (ISO,unixtime): " << std::endl;
        std::cin >> unit_choice;
        if (unit_choice == "unixtime") {
            time_format_param = "&timeformat=" + unit_choice;
        }
        else if (unit_choice != "ISO") {
            std::cerr << "Invalid time format.Defaulting to ISO." << std::endl;
        }
        std::string timezone;
        std::cout << "Enter timezone (e.g., GMT, EST): ";
        std::cin >> timezone;
        std::string past_days_format;
        std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
            past_days_format = " &past_days=" + unit_choice;
        }
        else if (unit_choice != "0") {
            std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
        }
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }

        std::string domain_param;
        std::cout << "Enter Domain (cams,cams_global,cams_europe): " << std::endl;
        std::cin >> unit_choice;
        if (unit_choice == "cams_global" || unit_choice == "cams_europe") {
            domain_param = "&domains=" + unit_choice;
        }
        else if (unit_choice != "cams") {
            std::cerr << "Invalid domain format.defaulting to cams" << std::endl;
        }

        std::string url = "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=" + std::to_string(latitude) +
            "&longitude=" + std::to_string(longitude) +"&hourly=european_aqi,european_aqi_pm2_5,european_aqi_pm10,european_aqi_nitrogen_dioxide,european_aqi_ozone,european_aqi_sulphur_dioxide" + time_format_param + "&timezone=" + timezone + forecast_days_format + domain_param;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            long http_response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
            std::cout << "HTTP Response Code: " << http_response_code << std::endl;
            // std::cout << "Response Data: " << response_data << std::endl; // Print response data for debugging
        }

        Document parsedData;
        parsedData.Parse(response_data.c_str());

        if (parsedData.HasParseError()) {
            std::cerr << "Error parsing JSON." << std::endl;
        }
        else {
            double latitude = parsedData["latitude"].GetDouble();
            double longitude = parsedData["longitude"].GetDouble();

            const Value& hourlyData = parsedData["hourly"];

            // Extract the required air quality data
            const Value& european_aqi = hourlyData["european_aqi"];
            const Value& european_aqi_pm2_5 = hourlyData["european_aqi_pm2_5"];
            const Value& european_aqi_pm10 = hourlyData["european_aqi_pm10"];
            const Value& european_aqi_nitrogen_dioxide = hourlyData["european_aqi_nitrogen_dioxide"];
            const Value& european_aqi_ozone = hourlyData["european_aqi_ozone"];
            const Value& european_aqi_sulphur_dioxide = hourlyData["european_aqi_sulphur_dioxide"];

            // Extract the time data
            const Value& hourlyTime = hourlyData["time"];

            // Output the air quality data
            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly European Air Quality Data:" << std::endl;
            for (SizeType i = 0; i < european_aqi.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl; // Print time
                std::cout << "European AQI: " << european_aqi[i].GetDouble() << std::endl;
                std::cout << "European AQI PM2.5: " << european_aqi_pm2_5[i].GetDouble() << std::endl;
                std::cout << "European AQI PM10: " << european_aqi_pm10[i].GetDouble() << std::endl;
                std::cout << "European AQI Nitrogen Dioxide: " << european_aqi_nitrogen_dioxide[i].GetDouble() << std::endl;
                std::cout << "European AQI Ozone: " << european_aqi_ozone[i].GetDouble() << std::endl;
                std::cout << "European AQI Sulphur Dioxide: " << european_aqi_sulphur_dioxide[i].GetDouble() << std::endl;

                // Add a gap after each hourly data set
                std::cout << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}
void Weather::viewAirData() {
    Weather weather;
    LocationManagement locationManager;
    Api api;
    Air air;
    int choice;

    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  View Any Location Air Data       \n";
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
            cout << "Which form of air data would you like to view:\n";
            cout << "\t1) Hourly Air Data Variables\n";
            cout << "\t2) European Air Data Variables\n";
            cout << "\t3) Return to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> weatherChoice;

            switch (weatherChoice) {
            case 1:
                air.fetchAndDisplayHourlyAirData(coordinates.first, coordinates.second);
                break;
            case 2:
                air.fetchAndDisplayEuropeanAirData(coordinates.first, coordinates.second);
                break;
            case 3:
                return; // Return to the main menu
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
//int main() {
//    Air air;
//    Api api;
//    std::string location;
//    std::cout << "Enter location: ";
//    std::cin >> location;
//
//    auto coordinates = api.fetchLocation(location);
//    std::cout << "Coordinates for " << location << ": Latitude = " << coordinates.first << ", Longitude = " << coordinates.second << std::endl;
//
//    air.fetchAndDisplayEuropeanAirData(coordinates.first, coordinates.second);
//
//    return 0;
//}
//
