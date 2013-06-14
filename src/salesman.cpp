#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

#include <cmath>
#include <cstdlib>

#include "GnuplotPipes.hpp"
#include "time.h"
#include "unistd.h"

using namespace std;

typedef map<int, pair<double,double> > CityMap;

CityMap cities;
pair<double,double> salesman;
double mutationRate = 0.2;
unsigned int popSize = 10, numCities = 9, sameFittestCounter = 0;
set<double> fitnessHistory;
//map<double,int> fitnesses;
vector<double> fitnesses;
vector<vector<double> > population; 

double CalcDistance(const pair<double,double> &a, const pair<double,double> &b) {
    double diffX = b.first - a.first;
    double diffY = b.second - b.second;
    return(sqrt(diffX*diffX + diffY*diffY));
}

void FillCities(CityMap &cities) {
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

int RndCity(void) {
    return( (double(rand())/RAND_MAX)* 9 );
}

double EvaluateFitness(const vector<double> &chromosome) {
    double fitness = 0;
    for(unsigned int i = 0; i < chromosome.size(); i++) {
        pair<double,double> city0 = (*cities.find(chromosome.at(i))).second;
        if(i == 0) {
            fitness += CalcDistance(salesman, city0);
        }else if(i == chromosome.size()-1) {
            fitness += CalcDistance(city0, salesman);
        }else {
            pair<double,double> city1 = (*cities.find(chromosome[i+1])).second;
            fitness += CalcDistance(city0, city1);
        }
    }
    return(fitness);
}

bool CheckInFitnesses(const double &fitness) {
    return(find(fitnesses.begin(), fitnesses.end(), fitness) != 
           fitnesses.end());
    //return(fitnessHistory.find(fitness) != fitnessHistory.end());
}

void GenerateInitialPopulation(void) {
    double fitness;
    while (population.size() != popSize) {
	vector<double> chromosome;
        while(chromosome.size() != numCities) {
	    int rndCity = RndCity();
	    if(find(chromosome.begin(), chromosome.end(), rndCity) == 
	       chromosome.end())
		chromosome.push_back(rndCity);
        }

        fitness = EvaluateFitness(chromosome);

        while(CheckInFitnesses(fitness)) {
            random_shuffle(chromosome.begin(), chromosome.end());
            fitness = EvaluateFitness(chromosome);
        }        

        fitnessHistory.insert(fitness);
        fitnesses.push_back(fitness);
        population.push_back(chromosome);
    }// while
}

double SalesmanStart(void) {
    return((double(rand())/(RAND_MAX))*40.);
}

int Round(const double &val)
{
    double intpart;
    if(modf(val, &intpart) < 0.5)
	return(floor(val));
    else
        return(ceil(val));
}

vector<double> PerformCrossover(vector<double> a, vector<double> b) {
    int halfNumCities = Round(numCities*0.5);
    vector<double> offspring;
    for(int i = 0; i < halfNumCities-1; i++) {
        offspring.push_back(a[i]);
        b.erase(find(b.begin(), b.end(), a[i]));
    }
    
    for(int i = 0; i < 2; i++)
        offspring.push_back(b[i]);
    
    for(int i = 0; i < offspring.size(); i++)
        a.erase(find(a.begin(), a.end(), offspring[i]));
    
    for(int i = 0; i < a.size(); i++)
        offspring.push_back(a[i]);
    
    double fitness = EvaluateFitness(offspring);

    while(CheckInFitnesses(fitness)) {
        random_shuffle(offspring.begin(), offspring.end());
        //next_permutation(offspring.begin()+halfNumCities, offspring.end());
        fitness = EvaluateFitness(offspring);
    }        

    fitnesses.push_back(fitness);
    fitnessHistory.insert(fitness);
    return(offspring);
}

int main (int argc, char* argv[]) {
    Gnuplot g1;
    g1.Cmd("set terminal gif animate delay 0.0001");
    vector<double> solution;
    vector<string> plottingFiles;
    double solutionFitness;
    int maxIterations = atoi(argv[1]);

    //Initialize the random number generator. 
    srand( time(NULL) );

    //Fill up the map with all the different cities.
    FillCities(cities);

    //Initialize the coordinates for the saleman
    //salesman = make_pair(SalesmanStart(), SalesmanStart());
    salesman = make_pair(0,0);
    
    //Generate the initial population. 
    GenerateInitialPopulation();

    int numIterations=0, numSolutions=0; 
    while(numIterations != maxIterations) {
#ifdef VERBOSE
        cout << "Iteration number : " << numIterations << endl;
#endif
        double fittest = 1e6, secFittest = 1e6, thirdFittest = 1e6;
        double fatBastard0 = 0, fatBastard1 = 0, fatBastard2 = 0;
        unsigned int fittestPos = -1, secPos = -1, thirdPos = -1;
        unsigned int bastardPos0 = -1, bastardPos1 = -1, bastardPos2 = -1;

        //Evaluate the fitness of each solution.
#ifdef VERBOSE
        cout << "The Current Population Fitnesses: " << endl;
#endif
        for(unsigned int i = 0; i < population.size(); i++) {
            double fitness = EvaluateFitness(population[i]);
#ifdef VERBOSE           
            cout << fitness << " " << i << "    " << fittest << " " 
                 << secFittest << " " << thirdFittest << " " ;
            for(unsigned int j = 0; j < population[i].size(); j++)
                cout << population[i][j] << " ";
            cout << endl;
#endif            
            if(fitness < fittest) {
                secFittest = fittest;
                secPos = fittestPos;
                
                fittest = fitness;
                fittestPos = i;
            }else if(fitness < secFittest && fitness > fittest) {
                thirdFittest = secFittest;
                thirdPos = secPos;

                secFittest=fitness;
                secPos = i;
            }else if(fitness < thirdFittest && fitness > secFittest) {
                thirdFittest=fitness;
                thirdPos = i;
            }
                
            if(fitness > fatBastard0) {
                fatBastard1 = fatBastard0;
                bastardPos1 = bastardPos0;

                fatBastard0 = fitness;
                bastardPos0 = i;
            }else if(fitness > fatBastard1 && fitness < fatBastard0) {
                fatBastard2 = fatBastard1;
                bastardPos2 = bastardPos1;

                fatBastard1 = fitness;
                bastardPos1 = i;
            }else if(fitness > fatBastard2 && fitness < fatBastard1) {
                fatBastard2 = fitness;
                bastardPos2 = i;
            }
        }//for(unsigned int i = 0; i < population.size();

#ifdef VERBOSE        
        cout << endl << "Fittest : " << fittest << " " << fittestPos
             << "  |  1st Fittest : " << secFittest << " " 
             << secPos << "  |  2nd Fittest : " << thirdFittest << " " 
             << thirdPos << endl
             << "Fattest Bastard: " << fatBastard0 << " " 
             << bastardPos0 << "  | 1st Fattest: " << fatBastard1 << " " 
             << bastardPos1 << "   | 2nd Fattest: " << fatBastard2 
             << " " << bastardPos2 << endl;
#endif        

        cout << endl << "The solution is " << fittest <<". After " 
             << numIterations << " iterations. " << endl;
        if(solutionFitness > fittest) {
            vector<double> temp = population.at(fittestPos);
            stringstream ss;
            ss << "/tmp/test" << numSolutions << ".dat";
            ofstream soln;
            plottingFiles.push_back(ss.str());
            soln.open(ss.str().c_str());
            for(unsigned int i = 0; i < temp.size(); i++) {
                pair<double,double> pair0 = (*cities.find(temp[i])).second;
                if(i == 0) {
                    pair<double,double> pair1 = (*cities.find(solution[i+1])).second;
                    soln << "Salesman " << salesman.first << " " << salesman.second << " " 
                         << pair0.first-salesman.first << " " 
                         << pair0.second-salesman.second << endl
                         << i << " " << pair0.first << " " << pair0.second << " " 
                         << pair1.first-pair0.first << " " << pair1.second-pair0.second 
                         << endl;
                }else if(i == solution.size()-1) {
                    soln << i << " " << pair0.first << " " << pair0.second << " " 
                         << salesman.first-pair0.first << " " 
                         << salesman.second-pair0.second << endl;
                }else {
                    pair<double,double> pair1 = (*cities.find(temp[i+1])).second;
                    soln << i << " " << pair0.first << " " << pair0.second << " " 
                         << pair1.first-pair0.first << " " << pair1.second-pair0.second 
                         << endl;
                }
            }
            soln.close();
            numSolutions++;
        }
        
        solution = population.at(fittestPos);
        solutionFitness = fittest;
        
        if(fittest <= 37.071)
            break;

        //The Mating Ritual
        vector<double> parentA = population.at(fittestPos);
        vector<double> parentB = population.at(secPos);
        vector<double> parentC = population.at(thirdPos);

        vector<double> offspring0 = PerformCrossover(parentA, parentB);
        vector<double> offspring1 = PerformCrossover(parentA, parentC);
        vector<double> offspring2 = PerformCrossover(parentB, parentC);
        
#ifdef VERBOSE
        // cout << "dfiajlofjweiojfeoj   ";
        // for(int i = 0; i < offspring0.size(); i++)
        //     cout << " " << offspring0[i];
        cout << endl << "---------------------------------------" << endl;
#endif
    
        //The mutations
        bool hasMutation = double(rand())/ RAND_MAX < mutationRate;

        if(hasMutation) {
            double fitness0 = EvaluateFitness(offspring0);
            while(CheckInFitnesses(fitness0)) {
                int city0 = RndCity();
                int city1 = RndCity();
                while(city0 == city1)
                    city0=RndCity();
                 
                vector<double>::iterator it0 = 
                    find(offspring0.begin(),offspring0.end(),RndCity());
                vector<double>::iterator it1 = 
                    find(offspring0.begin(),offspring0.end(),RndCity());

                swap(offspring0.at(city0),offspring0.at(city1));
                fitness0 = EvaluateFitness(offspring0);
            }
        }

        //And add the new guys
        population.at(bastardPos0) = offspring0;
        population.at(bastardPos1) = offspring1;
        population.at(bastardPos2) = offspring2;
        
        numIterations++;
    }//while 

    // cout << endl << "The solution is " << solutionFitness <<". After " 
    //      << numIterations << " iterations. " << endl;
    // ofstream soln;
    // soln.open("test.dat");
    // for(unsigned int i = 0; i < solution.size(); i++) {
    //     pair<double,double> pair0 = (*cities.find(solution[i])).second;
    //     if(i == 0) {
    //         pair<double,double> pair1 = (*cities.find(solution[i+1])).second;
    //         soln << "Salesman " << salesman.first << " " << salesman.second << " " 
    //              << pair0.first-salesman.first << " " 
    //              << pair0.second-salesman.second << endl
    //              << i << " " << pair0.first << " " << pair0.second << " " 
    //              << pair1.first-pair0.first << " " << pair1.second-pair0.second 
    //              << endl;
    //     }else if(i == solution.size()-1) {
    //         soln << i << " " << pair0.first << " " << pair0.second << " " 
    //              << salesman.first-pair0.first << " " 
    //              << salesman.second-pair0.second << endl;
    //     }else {
    //         pair<double,double> pair1 = (*cities.find(solution[i+1])).second;
    //         soln << i << " " << pair0.first << " " << pair0.second << " " 
    //              << pair1.first-pair0.first << " " << pair1.second-pair0.second 
    //              << endl;
    //     }
    // }
    // soln.close();


    g1.UnSetKey().SetRange("x",0,40);
    g1.Cmd("set output \"animated.gif\"");
    
    stringstream makeGif;
    for(int i = 0; i < numSolutions; i++) {
        stringstream temp;
        temp << "/tmp/test" << i << ".dat";

        makeGif << "set title \"Solution Number " << i << "\";plot \"" 
                << temp.str() << "\" u 2:3:4:5 w vector, \"" 
                << temp.str() << "\" u 2:3 w points pt 4;";
    }
    g1.Cmd(makeGif.str());
    //g1.Cmd("plot 'test.dat' u 2:3:4:5 w vector, 'test.dat' u 2:3 w points pt 4");
    sleep(1.5);
    // g1.WaitForKey();
    g1.RemoveTempFiles();

    for(unsigned int i = 0; i < plottingFiles.size(); i++)
}
