#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "WeatherAPI.h"

#include "exportDataChart.h"

using namespace rapidjson;

// Callback function to write the received data
size_t Api::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response_data) {
    size_t total_size = size * nmemb;
    response_data->append((char*)contents, total_size);
    return total_size;
}


// Function to fetch coordinates for a given location
std::pair<double, double> Api::fetchLocation(const std::string& location) {
    CURL* curl = curl_easy_init();
    std::pair<double, double> coordinates = { 0.0, 0.0 }; // Default coordinates in case of failure

    if (curl) {
        std::string url = "https://geocoding-api.open-meteo.com/v1/search?name=" + location + "&count=1&language=en&format=json";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to fetch location data. Error: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            Document parsedData;
            parsedData.Parse(response_data.c_str());

            if (!parsedData.HasParseError() && parsedData.HasMember("results") && parsedData["results"].IsArray()) {
                const auto& results = parsedData["results"].GetArray();
                if (!results.Empty()) {
                    const auto& result = results[0];
                    if (result.HasMember("latitude") && result["latitude"].IsNumber() &&
                        result.HasMember("longitude") && result["longitude"].IsNumber()) {
                        coordinates.first = result["latitude"].GetDouble();
                        coordinates.second = result["longitude"].GetDouble();
                    }
                }
            }
            else {
                std::cerr << "Failed to parse location data." << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }

    return coordinates;
}

// Function to fetch weather data and display it using coordinates

void Api::fetchAndDisplaySomeHourlyWeatherData(double latitude, double longitude) {
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
     /*   std::string past_days_format;
        std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
            past_days_format = " &past_days=" + unit_choice;
        }
        else if (unit_choice != "0") {
            std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
        }*/
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }


        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&hourly=temperature_2m,relative_humidity_2m,precipitation,pressure_msl,surface_pressure,cloud_cover" +
            temperature_unit_param +
            wind_speed_unit_param +
            precip_unit_param +
            time_format_param +
            "&timezone=" + timezone+forecast_days_format;
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
            const Value& hourlyPrecipitation = hourlyData["precipitation"];
            const Value& hourlyPressureMsl = hourlyData["pressure_msl"];
            const Value& hourlySurfacePressure = hourlyData["surface_pressure"];
            const Value& hourlyCloudCover = hourlyData["cloud_cover"];

            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Data:" << std::endl;
            for (SizeType i = 0; i < hourlyTime.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl;
                std::cout << "Temperature: " << hourlyTemperature[i].GetDouble() << std::endl;
                std::cout << "Relative Humidity: " << hourlyHumidity[i].GetDouble() << std::endl;
                std::cout << "Precipitation: " << hourlyPrecipitation[i].GetDouble() << std::endl;
                std::cout << "Pressure MSL: " << hourlyPressureMsl[i].GetDouble() << std::endl;
                std::cout << "Surface Pressure: " << hourlySurfacePressure[i].GetDouble() << std::endl;
                std::cout << "Cloud Cover: " << hourlyCloudCover[i].GetDouble() << std::endl;

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


void Api::fetchAndDisplayAdditionalData(double latitude, double longitude) {
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
        /*   std::string past_days_format;
        std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
            past_days_format = " &past_days=" + unit_choice;
        }
        else if (unit_choice != "0") {
            std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
        }*/
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }

        // Construct the URL with all parameters
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&hourly=soil_temperature_0cm,uv_index,uv_index_clear_sky,is_day,cape" +
            temperature_unit_param +
            wind_speed_unit_param +
            precip_unit_param +
            time_format_param +
            "&timezone=" + timezone+forecast_days_format;


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

            const Value& hourlySoilTemperature0cm = hourlyData["soil_temperature_0cm"];
            const Value& hourlyUvIndex = hourlyData["uv_index"];
            const Value& hourlyUvIndexClearSky = hourlyData["uv_index_clear_sky"];
            const Value& hourlyCape = hourlyData["cape"];

            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Additional Data:" << std::endl;
            for (SizeType i = 0; i < hourlySoilTemperature0cm.Size(); i++) {
                std::cout << "Soil Temperature 0cm: " << hourlySoilTemperature0cm[i].GetDouble() << std::endl;
                std::cout << "UV Index: " << hourlyUvIndex[i].GetDouble() << std::endl;
                std::cout << "UV Index Clear Sky: " << hourlyUvIndexClearSky[i].GetDouble() << std::endl;
                std::cout << "Cape: " << hourlyCape[i].GetDouble() << std::endl;

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


void Api::fetchAndDisplayAllHourlyWeatherData(double latitude, double longitude) {
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
        /*   std::string past_days_format;
      std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
      std::cin >> unit_choice;
      if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
          past_days_format = " &past_days=" + unit_choice;
      }
      else if (unit_choice != "0") {
          std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
      }*/
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }

        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,apparent_temperature,precipitation_probability,precipitation,rain,showers,snowfall,snow_depth,weather_code,pressure_msl,surface_pressure,cloud_cover,cloud_cover_low,cloud_cover_mid,cloud_cover_high,visibility,evapotranspiration,et0_fao_evapotranspiration,vapour_pressure_deficit,wind_speed_10m,wind_speed_80m,wind_speed_120m,wind_speed_180m,wind_direction_10m,wind_direction_80m,wind_direction_120m,wind_direction_180m,wind_gusts_10m,temperature_80m,temperature_120m,temperature_180m,soil_temperature_0cm,soil_temperature_6cm,soil_temperature_18cm,soil_temperature_54cm,soil_moisture_0_to_1cm,soil_moisture_1_to_3cm,soil_moisture_3_to_9cm,soil_moisture_9_to_27cm,soil_moisture_27_to_81cm"
            +wind_speed_unit_param + precip_unit_param + time_format_param +
            temperature_unit_param+ "&timezone=" + timezone + forecast_days_format;
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
            const Value& hourlyTemperature2m = hourlyData["temperature_2m"];
            const Value& hourlyRelativeHumidity2m = hourlyData["relative_humidity_2m"];
            const Value& hourlyDewPoint2m = hourlyData["dew_point_2m"];
            const Value& hourlyApparentTemperature = hourlyData["apparent_temperature"];
            const Value& hourlyPrecipitationProbability = hourlyData["precipitation_probability"];
            const Value& hourlyPrecipitation = hourlyData["precipitation"];
            const Value& hourlyRain = hourlyData["rain"];
            const Value& hourlyShowers = hourlyData["showers"];
            const Value& hourlySnowfall = hourlyData["snowfall"];
            const Value& hourlySnowDepth = hourlyData["snow_depth"];
            const Value& hourlyWeatherCode = hourlyData["weather_code"];
            const Value& hourlyPressureMsl = hourlyData["pressure_msl"];
            const Value& hourlySurfacePressure = hourlyData["surface_pressure"];
            const Value& hourlyCloudCover = hourlyData["cloud_cover"];
            const Value& hourlyCloudCoverLow = hourlyData["cloud_cover_low"];
            const Value& hourlyCloudCoverMid = hourlyData["cloud_cover_mid"];
            const Value& hourlyCloudCoverHigh = hourlyData["cloud_cover_high"];
            const Value& hourlyVisibility = hourlyData["visibility"];
            const Value& hourlyEvapotranspiration = hourlyData["evapotranspiration"];
            const Value& hourlyEt0FaoEvapotranspiration = hourlyData["et0_fao_evapotranspiration"];
            const Value& hourlyVapourPressureDeficit = hourlyData["vapour_pressure_deficit"];
            const Value& hourlyWindSpeed10m = hourlyData["wind_speed_10m"];
            const Value& hourlyWindSpeed80m = hourlyData["wind_speed_80m"];
            const Value& hourlyWindSpeed120m = hourlyData["wind_speed_120m"];
            const Value& hourlyWindSpeed180m = hourlyData["wind_speed_180m"];
            const Value& hourlyWindDirection10m = hourlyData["wind_direction_10m"];
            const Value& hourlyWindDirection80m = hourlyData["wind_direction_80m"];
            const Value& hourlyWindDirection120m = hourlyData["wind_direction_120m"];
            const Value& hourlyWindDirection180m = hourlyData["wind_direction_180m"];
            const Value& hourlyWindGusts10m = hourlyData["wind_gusts_10m"];
            const Value& hourlyTemperature80m = hourlyData["temperature_80m"];
            const Value& hourlyTemperature120m = hourlyData["temperature_120m"];
            const Value& hourlyTemperature180m = hourlyData["temperature_180m"];
            const Value& hourlySoilTemperature0cm = hourlyData["soil_temperature_0cm"];
            const Value& hourlySoilTemperature6cm = hourlyData["soil_temperature_6cm"];
            const Value& hourlySoilTemperature18cm = hourlyData["soil_temperature_18cm"];
            const Value& hourlySoilTemperature54cm = hourlyData["soil_temperature_54cm"];
            const Value& hourlySoilMoisture0To1cm = hourlyData["soil_moisture_0_to_1cm"];
            const Value& hourlySoilMoisture1To3cm = hourlyData["soil_moisture_1_to_3cm"];
            const Value& hourlySoilMoisture3To9cm = hourlyData["soil_moisture_3_to_9cm"];
            const Value& hourlySoilMoisture9To27cm = hourlyData["soil_moisture_9_to_27cm"];
            const Value& hourlySoilMoisture27To81cm = hourlyData["soil_moisture_27_to_81cm"];

            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Data:" << std::endl;
            for (SizeType i = 0; i < hourlyTime.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl;
                std::cout << "Temperature (2m): " << hourlyTemperature2m[i].GetDouble() << std::endl;
                std::cout << "Relative Humidity (2m): " << hourlyRelativeHumidity2m[i].GetDouble() << std::endl;
                std::cout << "Dew Point (2m): " << hourlyDewPoint2m[i].GetDouble() << std::endl;
                std::cout << "Apparent Temperature: " << hourlyApparentTemperature[i].GetDouble() << std::endl;
                std::cout << "Precipitation Probability: " << hourlyPrecipitationProbability[i].GetDouble() << std::endl;
                std::cout << "Precipitation: " << hourlyPrecipitation[i].GetDouble() << std::endl;
                std::cout << "Rain: " << hourlyRain[i].GetDouble() << std::endl;
                std::cout << "Showers: " << hourlyShowers[i].GetDouble() << std::endl;
                std::cout << "Snowfall: " << hourlySnowfall[i].GetDouble() << std::endl;
                std::cout << "Snow Depth: " << hourlySnowDepth[i].GetDouble() << std::endl;
                std::cout << "Weather Code: " << hourlyWeatherCode[i].GetDouble() << std::endl;
                std::cout << "Pressure MSL: " << hourlyPressureMsl[i].GetDouble() << std::endl;
                std::cout << "Surface Pressure: " << hourlySurfacePressure[i].GetDouble() << std::endl;
                std::cout << "Cloud Cover: " << hourlyCloudCover[i].GetDouble() << std::endl;
                std::cout << "Cloud Cover (Low): " << hourlyCloudCoverLow[i].GetDouble() << std::endl;
                std::cout << "Cloud Cover (Mid): " << hourlyCloudCoverMid[i].GetDouble() << std::endl;
                std::cout << "Cloud Cover (High): " << hourlyCloudCoverHigh[i].GetDouble() << std::endl;
                std::cout << "Visibility: " << hourlyVisibility[i].GetDouble() << std::endl;
                std::cout << "Evapotranspiration: " << hourlyEvapotranspiration[i].GetDouble() << std::endl;
                std::cout << "ET0 FAO Evapotranspiration: " << hourlyEt0FaoEvapotranspiration[i].GetDouble() << std::endl;
                std::cout << "Vapour Pressure Deficit: " << hourlyVapourPressureDeficit[i].GetDouble() << std::endl;
                std::cout << "Wind Speed (10m): " << hourlyWindSpeed10m[i].GetDouble() << std::endl;
                std::cout << "Wind Speed (80m): " << hourlyWindSpeed80m[i].GetDouble() << std::endl;
                std::cout << "Wind Speed (120m): " << hourlyWindSpeed120m[i].GetDouble() << std::endl;
                std::cout << "Wind Speed (180m): " << hourlyWindSpeed180m[i].GetDouble() << std::endl;
                std::cout << "Wind Direction (10m): " << hourlyWindDirection10m[i].GetDouble() << std::endl;
                std::cout << "Wind Direction (80m): " << hourlyWindDirection80m[i].GetDouble() << std::endl;
                std::cout << "Wind Direction (120m): " << hourlyWindDirection120m[i].GetDouble() << std::endl;
                std::cout << "Wind Direction (180m): " << hourlyWindDirection180m[i].GetDouble() << std::endl;
                std::cout << "Wind Gusts (10m): " << hourlyWindGusts10m[i].GetDouble() << std::endl;
                std::cout << "Temperature (80m): " << hourlyTemperature80m[i].GetDouble() << std::endl;
                std::cout << "Temperature (120m): " << hourlyTemperature120m[i].GetDouble() << std::endl;
                std::cout << "Temperature (180m): " << hourlyTemperature180m[i].GetDouble() << std::endl;
                std::cout << "Soil Temperature (0cm): " << hourlySoilTemperature0cm[i].GetDouble() << std::endl;
                std::cout << "Soil Temperature (6cm): " << hourlySoilTemperature6cm[i].GetDouble() << std::endl;
                std::cout << "Soil Temperature (18cm): " << hourlySoilTemperature18cm[i].GetDouble() << std::endl;
                std::cout << "Soil Temperature (54cm): " << hourlySoilTemperature54cm[i].GetDouble() << std::endl;
                std::cout << "Soil Moisture (0 to 1cm): " << hourlySoilMoisture0To1cm[i].GetDouble() << std::endl;
                std::cout << "Soil Moisture (1 to 3cm): " << hourlySoilMoisture1To3cm[i].GetDouble() << std::endl;
                std::cout << "Soil Moisture (3 to 9cm): " << hourlySoilMoisture3To9cm[i].GetDouble() << std::endl;
                std::cout << "Soil Moisture (9 to 27cm): " << hourlySoilMoisture9To27cm[i].GetDouble() << std::endl;
                std::cout << "Soil Moisture (27 to 81cm): " << hourlySoilMoisture27To81cm[i].GetDouble() << std::endl;
                std::cout << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}


void Api::fetchAndDisplayDailyWeatherData(double latitude, double longitude) {
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
        /*   std::string past_days_format;
        std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
            past_days_format = " &past_days=" + unit_choice;
        }
        else if (unit_choice != "0") {
            std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
        }*/
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }

        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&daily=weather_code,temperature_2m_max,temperature_2m_min,apparent_temperature_max,apparent_temperature_min,sunrise,sunset,daylight_duration,sunshine_duration,uv_index_max,uv_index_clear_sky_max,precipitation_sum,rain_sum,showers_sum,snowfall_sum,precipitation_hours,precipitation_probability_max,wind_speed_10m_max,wind_gusts_10m_max,wind_direction_10m_dominant,shortwave_radiation_sum,et0_fao_evapotranspiration"+wind_speed_unit_param + precip_unit_param + time_format_param
            +temperature_unit_param + "&timezone=" + timezone+forecast_days_format;
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
        }

        // Parse the JSON string
        Document parsedData;
        parsedData.Parse(response_data.c_str());

        if (parsedData.HasParseError()) {
            std::cerr << "Error parsing JSON." << std::endl;
        }
        else {
            double latitude = parsedData["latitude"].GetDouble();
            double longitude = parsedData["longitude"].GetDouble();

            // Access daily weather data
            const Value& dailyData = parsedData["daily"];
            double weatherCode = dailyData["weather_code"][0].GetDouble(); // Assuming the first day's data
            double maxTemperature = dailyData["temperature_2m_max"][0].GetDouble(); // Assuming the first day's data
            double minTemperature = dailyData["temperature_2m_min"][0].GetDouble();
            double apparentTemperatureMax = dailyData["apparent_temperature_max"][0].GetDouble();
            double apparentTemperatureMin = dailyData["apparent_temperature_min"][0].GetDouble();
            std::string sunrise = dailyData["sunrise"][0].GetString(); // Retrieve as string
            std::string sunset = dailyData["sunset"][0].GetString();
            double daylightDuration = dailyData["daylight_duration"][0].GetDouble();
            double sunshineDuration = dailyData["sunshine_duration"][0].GetDouble();
            double uvIndexMax = dailyData["uv_index_max"][0].GetDouble();
            double uvIndexClearSkyMax = dailyData["uv_index_clear_sky_max"][0].GetDouble();
            double precipitationSum = dailyData["precipitation_sum"][0].GetDouble();
            double rainSum = dailyData["rain_sum"][0].GetDouble();
            double showersSum = dailyData["showers_sum"][0].GetDouble();
            double snowfallSum = dailyData["snowfall_sum"][0].GetDouble();
            double precipitationHours = dailyData["precipitation_hours"][0].GetDouble();
            double precipitationProbabilityMax = dailyData["precipitation_probability_max"][0].GetDouble();
            double windSpeed10mMax = dailyData["wind_speed_10m_max"][0].GetDouble();
            double windGusts10mMax = dailyData["wind_gusts_10m_max"][0].GetDouble();
            double windDirection10mDominant = dailyData["wind_direction_10m_dominant"][0].GetDouble();
            double shortwaveRadiationSum = dailyData["shortwave_radiation_sum"][0].GetDouble();
            double et0FaoEvapotranspiration = dailyData["et0_fao_evapotranspiration"][0].GetDouble();

            std::cout << "latitude: " << latitude << std::endl;
            std::cout << "longitude: " << longitude << std::endl;
            std::cout << "weather code: " << weatherCode << std::endl;
            std::cout << "max temperature: " << maxTemperature << std::endl;
            std::cout << "min temperature: " << minTemperature << std::endl;
            std::cout << "apparent temperature max: " << apparentTemperatureMax << std::endl;
            std::cout << "apparent temperature min: " << apparentTemperatureMin << std::endl;
            std::cout << "sunrise: " << sunrise << std::endl;
            std::cout << "sunset: " << sunset << std::endl;
            std::cout << "daylight duration: " << daylightDuration << std::endl;
            std::cout << "sunshine duration: " << sunshineDuration << std::endl;
            std::cout << "UV index max: " << uvIndexMax << std::endl;
            std::cout << "UV index clear sky max: " << uvIndexClearSkyMax << std::endl;
            std::cout << "precipitation sum: " << precipitationSum << std::endl;
            std::cout << "rain sum: " << rainSum << std::endl;
            std::cout << "showers sum: " << showersSum << std::endl;
            std::cout << "snowfall sum: " << snowfallSum << std::endl;
            std::cout << "precipitation hours: " << precipitationHours << std::endl;
            std::cout << "precipitation probability max: " << precipitationProbabilityMax << std::endl;
            std::cout << "wind speed 10m max: " << windSpeed10mMax << std::endl;
            std::cout << "wind gusts 10m max: " << windGusts10mMax << std::endl;
            std::cout << "wind direction 10m dominant: " << windDirection10mDominant << std::endl;
            std::cout << "shortwave radiation sum: " << shortwaveRadiationSum << std::endl;
            std::cout << "et0 FAO evapotranspiration: " << et0FaoEvapotranspiration << std::endl;

        }

        curl_easy_cleanup(curl);
    }
}


void Api::fetchAndDisplaySolarRadiationData(double latitude, double longitude) {
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
        /*   std::string past_days_format;
        std::cout << "Enter Past Days(e.g,0,1,2,3,5):";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "2" || unit_choice == "3" || unit_choice == "5") {
            past_days_format = " &past_days=" + unit_choice;
        }
        else if (unit_choice != "0") {
            std::cerr << "Invalid past day format.Defaulting to 0." << std::endl;
        }*/
        std::string forecast_days_format;
        std::cout << "Enter Forecast Days(1,3,7,14,16): ";
        std::cin >> unit_choice;
        if (unit_choice == "1" || unit_choice == "3" || unit_choice == "14" || unit_choice == "16") {
            forecast_days_format = "&forecast_days=" + unit_choice;
        }
        else if (unit_choice != "7") {
            std::cerr << "Invalid forecast day.Defaulting to 7" << std::endl;
        }
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&hourly=shortwave_radiation,direct_radiation,diffuse_radiation,direct_normal_irradiance,global_tilted_irradiance,terrestrial_radiation,shortwave_radiation_instant,direct_radiation_instant,diffuse_radiation_instant,direct_normal_irradiance_instant,global_tilted_irradiance_instant,terrestrial_radiation_instant&timezone=" + timezone +
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
            const Value& shortwaveRadiation = hourlyData["shortwave_radiation"];
            const Value& directRadiation = hourlyData["direct_radiation"];
            const Value& diffuseRadiation = hourlyData["diffuse_radiation"];
            const Value& directNormalIrradiance = hourlyData["direct_normal_irradiance"];
            const Value& globalTiltedIrradiance = hourlyData["global_tilted_irradiance"];
            const Value& terrestrialRadiation = hourlyData["terrestrial_radiation"];
            const Value& shortwaveRadiationInstant = hourlyData["shortwave_radiation_instant"];
            const Value& directRadiationInstant = hourlyData["direct_radiation_instant"];
            const Value& diffuseRadiationInstant = hourlyData["diffuse_radiation_instant"];
            const Value& directNormalIrradianceInstant = hourlyData["direct_normal_irradiance_instant"];
            const Value& globalTiltedIrradianceInstant = hourlyData["global_tilted_irradiance_instant"];
            const Value& terrestrialRadiationInstant = hourlyData["terrestrial_radiation_instant"];

            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Solar Radiation Data:" << std::endl;
            for (SizeType i = 0; i < hourlyTime.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl;
                std::cout << "Shortwave Radiation: " << shortwaveRadiation[i].GetDouble() << std::endl;
                std::cout << "Direct Radiation: " << directRadiation[i].GetDouble() << std::endl;
                std::cout << "Diffuse Radiation: " << diffuseRadiation[i].GetDouble() << std::endl;
                std::cout << "Direct Normal Irradiance: " << directNormalIrradiance[i].GetDouble() << std::endl;
                std::cout << "Global Tilted Irradiance: " << globalTiltedIrradiance[i].GetDouble() << std::endl;
                std::cout << "Terrestrial Radiation: " << terrestrialRadiation[i].GetDouble() << std::endl;
                std::cout << "Shortwave Radiation (Instant): " << shortwaveRadiationInstant[i].GetDouble() << std::endl;
                std::cout << "Direct Radiation (Instant): " << directRadiationInstant[i].GetDouble() << std::endl;
                std::cout << "Diffuse Radiation (Instant): " << diffuseRadiationInstant[i].GetDouble() << std::endl;
                std::cout << "Direct Normal Irradiance (Instant): " << directNormalIrradianceInstant[i].GetDouble() << std::endl;
                std::cout << "Global Tilted Irradiance (Instant): " << globalTiltedIrradianceInstant[i].GetDouble() << std::endl;
                std::cout << "Terrestrial Radiation (Instant): " << terrestrialRadiationInstant[i].GetDouble() << std::endl;
                std::cout << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}


void Api::fetchAndDisplayPressureData(double latitude, double longitude) {
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
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
            std::to_string(latitude) + "&longitude=" +
            std::to_string(longitude) +
            "&hourly=temperature_1000hPa,relative_humidity_1000hPa,cloud_cover_1000hPa,windspeed_1000hPa,winddirection_1000hPa,geopotential_height_1000hPa&timezone=" + timezone +
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
            const Value& temperature1000hPa = hourlyData["temperature_1000hPa"];
            const Value& relativeHumidity1000hPa = hourlyData["relative_humidity_1000hPa"];
            const Value& cloudCover1000hPa = hourlyData["cloud_cover_1000hPa"];
            const Value& windSpeed1000hPa = hourlyData["windspeed_1000hPa"];
            const Value& windDirection1000hPa = hourlyData["winddirection_1000hPa"];
            const Value& geopotentialHeight1000hPa = hourlyData["geopotential_height_1000hPa"];

            std::cout << "Latitude: " << latitude << std::endl;
            std::cout << "Longitude: " << longitude << std::endl;

            std::cout << "Hourly Pressure Data:" << std::endl;
            for (SizeType i = 0; i < hourlyTime.Size(); i++) {
                std::cout << "Time: " << hourlyTime[i].GetString() << std::endl;
                std::cout << "Temperature (1000hPa): " << temperature1000hPa[i].GetDouble() << std::endl;
                std::cout << "Relative Humidity (1000hPa): " << relativeHumidity1000hPa[i].GetDouble() << std::endl;
                std::cout << "Cloud Cover (1000hPa): " << cloudCover1000hPa[i].GetDouble() << std::endl;
                std::cout << "Wind Speed (1000hPa): " << windSpeed1000hPa[i].GetDouble() << std::endl;
                std::cout << "Wind Direction (1000hPa): " << windDirection1000hPa[i].GetDouble() << std::endl;
                std::cout << "Geopotential Height (1000hPa): " << geopotentialHeight1000hPa[i].GetDouble() << std::endl;

                std::cout << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}
void Api::fetchAndDisplayModelData(double latitude, double longitude) {
    CURL* curl = curl_easy_init();

    if (curl) {
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(latitude) +
            "&longitude=" + std::to_string(longitude) +
            "&models=gfs_seamless,";

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
                std::cerr << "Error parsing JSON: " << (parsedData.GetParseError()) << std::endl;
            }
          /*  std::cout << response_data << std::endl;*/
            else  {
                double latitude = parsedData["latitude"].GetDouble();
                double longitude = parsedData["longitude"].GetDouble();
               
                std::cout << "Latitude: " << latitude << std::endl;
                std::cout << "Longtitude: " << longitude << std::endl;
                std::cout << "Generation Time:"<<parsedData["generationtime_ms"].GetDouble() << std::endl;
                std::cout<<"Utc Offset Seconds: "<<parsedData["utc_offset_seconds"].GetInt() << std::endl;
                std::cout <<"Timezone:"<< parsedData["timezone"].GetString() << std::endl;
                std::cout<<"Timezone Abbreviation:"<<parsedData["timezone_abbreviation"].GetString() << std::endl;
                std::cout <<"Elevation:"<< parsedData["elevation"].GetDouble() << std::endl;







             }
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}



//int main() {
//    SettingsPage settings;
//    settings.setUnits("Celsius", "km/h", "mm");
//    Api api;
//    
//    std::string location;
//    std::cout << "Enter location: ";
//    std::cin >> location;
//
//    auto coordinates = api.fetchLocation(location);
//    std::cout << "Coordinates for " << location << ": Latitude = " << coordinates.first << ", Longitude = " << coordinates.second << std::endl;
//
//    api.fetchAndDisplayModelData(coordinates.first, coordinates.second);
//  
//
//
//    return 0;
//}