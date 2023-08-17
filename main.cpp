#include <iostream>
#include <fstream>
#include "routers.h"

//#define INPUTFILE "input_without_end.txt"
//#define INPUTFILE "input_title.txt"
//#define INPUTFILE "input_empty.txt"
#define INPUTFILE "input.txt"
//#define INPUTFILE "input_3point.txt"

using namespace std;

int main()
{
    cout << "It's work" << endl;

    fstream flIn(INPUTFILE, fstream::in);
    string strPointDepart;
    string strDestin;

    flIn >> strPointDepart >> strDestin;

    if(!flIn.is_open()){
        cout << "File is not opened!" << endl;
        return -1;
    }

    cout << "Point depature: " << strPointDepart << endl;
    cout << "Destination: " << strDestin << endl;

    TripAndFare trip;
    VecTripFare vecTrips;

    while(true)
    {
        flIn >> trip.m_strPointDepatr >> trip.m_strDestinat >> trip.m_flFare;
        if(flIn.eof()) break;
        vecTrips.push_back(trip);
    }

    for(auto& trip: vecTrips)
    {
        cout << trip.m_strPointDepatr << "\t" << trip.m_strDestinat << "\t" << trip.m_flFare << endl;
    }
    cout << "----------------------------------" << endl;

    CRoutes routes;
    CRouteBest routeBest = routes.buildRouter(strPointDepart, strDestin, vecTrips);
    cout << endl;
    routes.printBestRoutes(routeBest);

    return 0;
}
