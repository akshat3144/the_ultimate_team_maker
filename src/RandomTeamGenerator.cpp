#include "../include/RandomTeamGenerator.h"
#include <fstream>
#include <sstream>
#include <random>
using namespace std;

void RandomTeamGenerator::readPersonsFromFile(const string &filename, const vector<int> &categoryIndices, const vector<double> &weight)
{
    string line;
    ifstream file(filename);

    // Skip the header line
    getline(file, line);

    // Process the data lines
    Persons.clear(); // Clear the existing Persons
    while (getline(file, line))
    {
        istringstream ss(line);
        string name;
        getline(ss, name, ',');
        Persons.push_back(Person(name, 0.0)); // Scores are not used for random teams
    }
}

vector<Team> RandomTeamGenerator::createRandomTeams(int numTeams)
{
    random_device rd;
    mt19937 gen(rd());

    vector<Team> teams(numTeams);
    int PersonIndex = 0;
    for (const auto &Person : Persons)
    {
        int teamIndex = PersonIndex % numTeams;
        teams[teamIndex].addPerson(Person);
        PersonIndex++;
    }

    return teams;
}