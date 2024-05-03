#include "Weather.h"

using namespace std;

void Weather::menu() {
    Weather weather;

    int choice;



    do {
        cout << "\033[2J\033[1;1H"; // Clear screen
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t  Weather Application Main Menu   \n";
        cout << "\t\t\t\t                               \n";
        cout << "\t\t\t\t________________________________\n";
        cout << "\t\t\t\t  1) Add Location           |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t  2) Set Favourite Location |     |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t  3) View Location Data     |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t  4) View Air Quality Data   |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t  5) Export  Data|\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t  6)View Historical  |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\t\t\t\t  7) Exit            |\n";
        cout << "\t\t\t\t                     |\n";
        cout << "\n\t\t\t Please choose one: ";
        cin >> choice;

        switch (choice) {
        case 1:


        {
             weather.addLocation();
        }
       
        break;
       case 2: {
            weather.setFavourite();

        }
              break;
        case 3: {
            weather.viewLocation();
        }
              break;
        case 4: {
            weather.viewAirData();

        }
              break;
       case 5: {
            weather.exportDataChart();
        }
              break;
       case 6: {
           weather.viewHistorical();
       }
             break;

        case 7: {
            cout << "Exiting ";

        }break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (true);
}


