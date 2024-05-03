#pragma once
#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"
class Export {
public:
	void exportCSV(double latitude, double longitude);
	void exportJSON(double latitude, double longitude);
	void exportXLSXE(double latitude, double longitude);
	
 };
