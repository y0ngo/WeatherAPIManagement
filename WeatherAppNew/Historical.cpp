#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "WeatherAPI.h"
#include "Historical.h"
#include "Weather.h"
#include "locationManagement.h"

using namespace rapidjson;

// Callback function to write the received data
size_t hApi::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response_data) {
    size_t total_size = size * nmemb;
    response_data->append((char*)contents, total_size);
    return total_size;
}
void hApi::fetchAndDisplaySomeHistoricalHourlyWeatherData(double latitude, double longitude, std::string start_date, std::string end_date) {
    CURL* curl = curl_easy_init();

    if (curl) {
        std::string unit_choice;
        std::cout << "Choose temperature unit (celsius/fahrenheit): ";
        std::cin >> unit_choice;

        std::string temperature_unit_param;
        if (unit_choice == "fahrenheit") {
            temperature_unit_param = "&temperature_unit=fahrenheit";
        }
        else if (unit_choice != "celsius") {
            std::cerr << "Invalid temperature unit. Defaulting to Celsius." << std::endl;
        }

        std::cout << "Choose wind speed unit (ms, km/h, mph, knots): ";
        std::cin >> unit_choice;

        std::string wind_speed_unit_param;
        if (unit_choice == "ms" || unit_choice == "mph" || unit_choice == "kn") {
            wind_speed_unit_param = "&wind_speed_unit=" + unit_choice;
        }
        else if (unit_choice != "km/h") {
            std::cerr << "Invalid wind speed unit. Defaulting to km/h." << std::endl;
        }
        std::string precip_unit_param;
        std::cout << "Choose precip unit (mm,inch): ";
        std::cin >> unit_choice;
        if (unit_choice == "inch") {
            precip_unit_param = "&precipitation_unit=" + unit_choice;
        }
        else if (unit_choice != "mm") {
            std::cerr << "Invalid precip unit.Defaulting to mm." << std::endl;
        }
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
        std::string url = "https://archive-api.open-meteo.com/v1/archive?latitude=" + std::to_string(latitude) +
            "&longitude=" + std::to_string(longitude) +
            "&start_date=" + start_date +
            "&end_date=" + end_date +
            "&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,apparent_temperature,precipitation,rain,snowfall,snow_depth,weather_code,pressure_msl,wind_speed_10m,soil_temperature_0_to_7cm&timezone=" + timezone +
            wind_speed_unit_param + precip_unit_param + time_format_param +
            temperature_unit_param;
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
            std::cout << "Response Data: " << response_data << std::endl; // Print response data for debugging
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

            const Value& hourlyTime = hourlyData["time"];
            const Value& hourlyTemperature = hourlyData["temperature_2m"];
            const Value& hourlyHumidity = hourlyData["relative_humidity_2m"];
            const Value& hourlyDewPoint = hourlyData["dew_point_2m"];
            const Value& hourlyApparentTemperature = hourlyData["apparent_temperature"];
           const Value& hourlyPrecipitation = hourlyData["precipitation"];
            const Value& hourlyRain = hourlyData["rain"];
            const Value& hourlySnowfall = hourlyData["snowfall"];
        //    const Value& hourlySnowDepth = hourlyData["snow_depth"];
          const Value& hourlyWeatherCode = hourlyData["weather_code"];
          const Value& hourlyPressureMSL = hourlyData["pressure_msl"];
            const Value& hourlyWindSpeed10m = hourlyData["wind_speed_10m"];
            const Value& hourlySoilTemperature0To7cm = hourlyData["soil_temperature_0_to_7cm"];

            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Data:" << std::endl;
            for (SizeType i = 0; i < hourlyTime.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl;
                std::cout << "Temperature: " << hourlyTemperature[i].GetDouble() << std::endl;
                std::cout << "Humidity: " << hourlyHumidity[i].GetDouble() << std::endl;
                std::cout << "Dew Point: " << hourlyDewPoint[i].GetDouble() << std::endl;
                std::cout << "Apparent Temperature: " << hourlyApparentTemperature[i].GetDouble() << std::endl;
               std::cout << "Precipitation: " << hourlyPrecipitation[i].GetDouble() << std::endl;
                std::cout << "Rain: " << hourlyRain[i].GetDouble() << std::endl;
                std::cout << "Snowfall: " << hourlySnowfall[i].GetDouble() << std::endl;
          //      std::cout << "Snow Depth: " << hourlySnowDepth[i].GetDouble() << std::endl;
                std::cout << "Weather Code: " << hourlyWeatherCode[i].GetInt() << std::endl;
                std::cout << "Pressure MSL: " << hourlyPressureMSL[i].GetDouble() << std::endl;
                std::cout << "Wind Speed 10m: " << hourlyWindSpeed10m[i].GetDouble() << std::endl;
               std::cout << "Soil Temperature 0-7cm: " << hourlySoilTemperature0To7cm[i].GetDouble() << std::endl;

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

void hApi::fetchAndDisplayHistoricalDailyWeatherData(double latitude, double longitude, std::string start_date, std::string end_date) {
    CURL* curl = curl_easy_init();

    if (curl) {
           std::string unit_choice;
        std::cout << "Choose temperature unit (celsius/fahrenheit): ";
        std::cin >> unit_choice;

        std::string temperature_unit_param;
        if (unit_choice == "fahrenheit") {
            temperature_unit_param = "&temperature_unit=fahrenheit";
        }
        else if (unit_choice != "celsius") {
            std::cerr << "Invalid temperature unit. Defaulting to Celsius." << std::endl;
        }

        std::cout << "Choose wind speed unit (ms, km/h, mph, knots): ";
        std::cin >> unit_choice;

        std::string wind_speed_unit_param;
        if (unit_choice == "ms" || unit_choice == "mph" || unit_choice == "kn") {
            wind_speed_unit_param = "&wind_speed_unit=" + unit_choice;
        }
        else if (unit_choice != "km/h") {
            std::cerr << "Invalid wind speed unit. Defaulting to km/h." << std::endl;
        }
        std::string precip_unit_param;
        std::cout << "Choose precip unit (mm,inch): ";
        std::cin >> unit_choice;
        if (unit_choice == "inch") {
            precip_unit_param = "&precipitation_unit=" + unit_choice;
        }
        else if (unit_choice != "mm") {
            std::cerr << "Invalid precip unit.Defaulting to mm." << std::endl;
        }
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
        std::string url = "https://archive-api.open-meteo.com/v1/archive?latitude=" + std::to_string(latitude) +
            "&longitude=" + std::to_string(longitude) +
            "&start_date=" + start_date +
            "&end_date=" + end_date +
            "&daily=weather_code,temperature_2m_max,temperature_2m_min,temperature_2m_mean,sunrise,sunset,daylight_duration,sunshine_duration,precipitation_sum,rain_sum,precipitation_hours,wind_speed_10m_max&timezone=" + timezone +
            wind_speed_unit_param + precip_unit_param + time_format_param +
            temperature_unit_param;;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        Document parsedData;
        parsedData.Parse(response_data.c_str());

        if (parsedData.HasParseError()) {
            std::cerr << "Error parsing JSON." << std::endl;
        }
        else {
            double latitude = parsedData["latitude"].GetDouble();
            double longitude = parsedData["longitude"].GetDouble();

            const Value& dailyData = parsedData["daily"];

            for (SizeType i = 0; i < dailyData["time"].Size(); i++) {
                double weatherCode = dailyData["weather_code"][i].GetDouble();
                double maxTemperature = dailyData["temperature_2m_max"][i].GetDouble();
                double minTemperature = dailyData["temperature_2m_min"][i].GetDouble();
                const char* sunrise = dailyData["sunrise"][i].GetString();
                const char* sunset = dailyData["sunset"][i].GetString();
                double daylightDuration = dailyData["daylight_duration"][i].GetDouble();
                double sunshineDuration = dailyData["sunshine_duration"][i].GetDouble();
                double precipitationSum = dailyData["precipitation_sum"][i].GetDouble();
                double rainSum = dailyData["rain_sum"][i].GetDouble();
                double precipitationHours = dailyData["precipitation_hours"][i].GetDouble();
                double windSpeed10mMax = dailyData["wind_speed_10m_max"][i].GetDouble();

                std::cout << "Day " << i + 1 << ":" << std::endl;
                std::cout << "Latitude: " << latitude << std::endl;
                std::cout << "Longitude: " << longitude << std::endl;
                std::cout << "Weather Code: " << weatherCode << std::endl;
                std::cout << "Max Temperature: " << maxTemperature << std::endl;
                std::cout << "Min Temperature: " << minTemperature << std::endl;
                std::cout << "Sunrise: " << sunrise << std::endl;
                std::cout << "Sunset: " << sunset << std::endl;
                std::cout << "Daylight Duration: " << daylightDuration << std::endl;
                std::cout << "Sunshine Duration: " << sunshineDuration << std::endl;
                std::cout << "Precipitation Sum: " << precipitationSum << std::endl;
                std::cout << "Rain Sum: " << rainSum << std::endl;
                std::cout << "Precipitation Hours: " << precipitationHours << std::endl;
                std::cout << "Max Wind Speed 10m: " << windSpeed10mMax << std::endl;

                // Add a gap after each daily data set
                std::cout << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }

}
void hApi::fetchAndDisplayAdditionalHistoricalData(double latitude, double longitude, std::string start_date, std::string end_date) {
    CURL* curl = curl_easy_init();

    if (curl) {
        // Construct the URL with provided parameters
        std::string url = "https://archive-api.open-meteo.com/v1/archive?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&start_date=" + start_date +
            "&end_date=" + end_date +
            "&hourly=is_day,sunshine_duration";

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

            Document parsedData;
            parsedData.Parse(response_data.c_str());

            if (parsedData.HasParseError()) {
                std::cerr << "Error parsing JSON." << std::endl;
            }
            else {
                double latitude = parsedData["latitude"].GetDouble();
                double longitude = parsedData["longitude"].GetDouble();

                const Value& hourlyData = parsedData["hourly"];

                const Value& hourlyIsDay = hourlyData["is_day"];
                const Value& hourlySunshineDuration = hourlyData["sunshine_duration"];

                std::cout << "Latitude: " << latitude << std::endl;
                std::cout << "Longitude: " << longitude << std::endl;

                std::cout << "Historical Additional Data:" << std::endl;
                for (SizeType i = 0; i < hourlyIsDay.Size(); i++) {
                    if (hourlyIsDay[i].IsBool()) {
                        std::cout << "Is Day: " << (hourlyIsDay[i].GetBool() ? "Yes" : "No") << std::endl;
                    }
                    else {
                        std::cerr << "Error: Unexpected format for 'is_day' field." << std::endl;
                    }

                    if (hourlySunshineDuration[i].IsDouble()) {
                        std::cout << "Sunshine Duration: " << hourlySunshineDuration[i].GetDouble() << " minutes" << std::endl;
                    }
                    else {
                        std::cerr << "Error: Unexpected format for 'sunshine_duration' field." << std::endl;
                    }

                    // Add a gap after each hourly data set
                    std::cout << std::endl;
                }
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}



void hApi::fetchAndDisplayHistoricalModelData(double latitude, double longitude, std::string start_date, std::string end_date) {
    CURL* curl = curl_easy_init();

    if (curl) {
        std::string url = "https://archive-api.open-meteo.com/v1/archive?start_date=" +
            start_date + "&end_date=" + end_date +
            "&latitude=" + std::to_string(latitude) + "&longitude=" + std::to_string(longitude) +
            "&models=ecmwf_ifs";

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

            // Parse JSON response
            Document parsedData;
            parsedData.Parse(response_data.c_str());


            if (parsedData.HasParseError()) {
                std::cerr << "Error parsing JSON: " << (parsedData.GetParseError()) << std::endl;
            }
            /*  std::cout << response_data << std::endl;*/
            else {
                double latitude = parsedData["latitude"].GetDouble();
                double longitude = parsedData["longitude"].GetDouble();

                std::cout << "Latitude: " << latitude << std::endl;
                std::cout << "Longtitude: " << longitude << std::endl;
                std::cout << "Generation Time:" << parsedData["generationtime_ms"].GetDouble() << std::endl;
                std::cout << "Utc Offset Seconds: " << parsedData["utc_offset_seconds"].GetInt() << std::endl;
                std::cout << "Timezone:" << parsedData["timezone"].GetString() << std::endl;
                std::cout << "Timezone Abbreviation:" << parsedData["timezone_abbreviation"].GetString() << std::endl;
                std::cout << "Elevation:" << parsedData["elevation"].GetDouble() << std::endl;







            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}

void Weather::viewHistorical() {
    hApi historicalApi;
    Api api;
    int choice;

    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  Choose Historical Data Option       \n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t  1) View Some Historical Hourly Weather Data  |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  2) View Historical Daily Weather Data |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  3) View Historical Model Data |\n";
        cout << "\t\t\t\t                            |\n";
        cout << "\t\t\t\t  4) Return to Main Menu    |\n";
        cout << "\t\t\t\t                            |\n";
        cin >> choice;

        switch (choice) {
        case 1: {
            std::string locationName;
            cout << "Enter location: ";
            cin >> locationName;

            auto coordinates = api.fetchLocation(locationName);

            if (coordinates.first == 0.0 && coordinates.second == 0.0) {
                cout << "Location not found. Please try again." << endl;
                break;
            }

            cout << "Coordinates for " << locationName << ": Latitude = " << coordinates.first << ", Longitude = " << coordinates.second << endl;

            // Fetch and display some historical hourly weather data
            std::string start_date, end_date;
            cout << "Enter start date (YYYY-MM-DD): ";
            cin >> start_date;
            cout << "Enter end date (YYYY-MM-DD): ";
            cin >> end_date;
            historicalApi.fetchAndDisplaySomeHistoricalHourlyWeatherData(coordinates.first, coordinates.second, start_date, end_date);
            break;
        }
        case 2: {
            std::string locationName;
            cout << "Enter location: ";
            cin >> locationName;

            auto coordinates = api.fetchLocation(locationName);

            if (coordinates.first == 0.0 && coordinates.second == 0.0) {
                cout << "Location not found. Please try again." << endl;
                break;
            }

            cout << "Coordinates for " << locationName << ": Latitude = " << coordinates.first << ", Longitude = " << coordinates.second << endl;

            // Fetch and display historical daily weather data
            std::string start_date, end_date;
            cout << "Enter start date (YYYY-MM-DD): ";
            cin >> start_date;
            cout << "Enter end date (YYYY-MM-DD): ";
            cin >> end_date;
            historicalApi.fetchAndDisplayHistoricalDailyWeatherData(coordinates.first, coordinates.second, start_date, end_date);
            break;
        }
        case 3: {
            std::string locationName;
            cout << "Enter location: ";
            cin >> locationName;

            auto coordinates = api.fetchLocation(locationName);

            if (coordinates.first == 0.0 && coordinates.second == 0.0) {
                cout << "Location not found. Please try again." << endl;
                break;
            }

            cout << "Coordinates for " << locationName << ": Latitude = " << coordinates.first << ", Longitude = " << coordinates.second << endl;

            // Fetch and display historical model data
            std::string start_date, end_date;
            cout << "Enter start date (YYYY-MM-DD): ";
            cin >> start_date;
            cout << "Enter end date (YYYY-MM-DD): ";
            cin >> end_date;
            historicalApi.fetchAndDisplayHistoricalModelData(coordinates.first, coordinates.second, start_date, end_date);
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



//
//int main() {
//    // Example usage
//    hApi hapi;
//    hapi.fetchAndDisplayHistoricalModelData(51.5085, -0.12574, "2024-03-13", "2024-03-27");
//    return 0;
//}