#pragma once
#ifndef WEATHER_H
#define WEATHER_H

#include <iostream>
#include <iomanip>

class Weather {
public:
    void menu();
    void addLocation();
    void setFavourite();
    void viewLocation();
    void viewAirData();
    void settings();
    void exportDataChart();
    void viewHistorical();
    
};

#endif
