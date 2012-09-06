#include <iostream>
#include <map>
#include <string>

#include <cmath>
#include <cstdlib>

#include "time.h"

using namespace std;

int main (int argc, char* argv[]) {
    srand( time(NULL) );

    map<string, pair<double,double> > cities;
    cities.insert(make_pair("alpha", make_pair(32.121,10.008)));
    cities.insert(make_pair("beta", make_pair(19.244, 31.93)));
    cities.insert(make_pair("gamma", make_pair(13.081,21.989)));
    cities.insert(make_pair("delta", make_pair(4.95,20.773)));
    cities.insert(make_pair("epsilon", make_pair(10.456,12.144)));
    cities.insert(make_pair("zeta", make_pair(23.779,1.737)));
    cities.insert(make_pair("eta", make_pair(23.435,16.621)));
    cities.insert(make_pair("theta", make_pair(14.137,28.683)));
    cities.insert(make_pair("iota", make_pair(28.826,22.07)));

    cout << random() << " " << random() << " " << random() << endl;




}
