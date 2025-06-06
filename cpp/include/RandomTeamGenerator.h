#ifndef RANDOMTEAMGENERATOR_H
#define RANDOMTEAMGENERATOR_H

#include "Generator.h"
#include "Team.h"
#include <vector>
#include <string>
using namespace std;

// Class to create random teams
class RandomTeamGenerator : public Generator
{
public:
    void readPersonsFromFile(const string &filename, const vector<int> &categoryIndices = {}, const vector<double> &weights = {});
    vector<Team> createRandomTeams(int numTeams);
};

#endif // RANDOMTEAMGENERATOR_H