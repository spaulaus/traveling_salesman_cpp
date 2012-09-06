#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <set>

#include <cmath>
#include <cstdlib>

#include "time.h"

using namespace std;

typedef map<int, pair<double,double> >::iterator City;

void ExportSalesman(const pair<double,double> &loc) {
    ofstream out;
    out.open("salesman.dat");
    out << loc.first << " " << loc.second;
    out.close();
}

void FillCities(map<int, pair<double,double> > &cities) {
    cities.insert(make_pair(0, make_pair(32.121,10.008)));
    cities.insert(make_pair(1, make_pair(19.244, 31.93)));
    cities.insert(make_pair(2, make_pair(13.081,21.989)));
    cities.insert(make_pair(3, make_pair(4.95,20.773)));
    cities.insert(make_pair(4, make_pair(10.456,12.144)));
    cities.insert(make_pair(5, make_pair(23.779,1.737)));
    cities.insert(make_pair(6, make_pair(23.435,16.621)));
    cities.insert(make_pair(7, make_pair(14.137,28.683)));
    cities.insert(make_pair(8, make_pair(28.826,22.07)));
}

double SalesmanStart(void) {
    return((double(rand())/(RAND_MAX))*40.);
}

double CalcDistance(pair<double,double> &a, pair<double,double> &b) {
    double diffX = b.first - a.first;
    double diffY = b.second - b.second;
    return(sqrt(diffX*diffX + diffY*diffY));
}

int main (int argc, char* argv[]) {
    //Initialize the random number generator. 
    srand( time(NULL) );

    //Fill up the map with all the different cities.
    map<int, pair<double,double> > cities;
    FillCities(cities);

    //Initialize the coordinates for the saleman
    pair<double,double> salesman = make_pair(SalesmanStart(), SalesmanStart());
    ExportSalesman(salesman);
}
