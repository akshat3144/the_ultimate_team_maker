#ifndef RANDOMCATEGORICALTEAMGENERATOR_H
#define RANDOMCATEGORICALTEAMGENERATOR_H

#include "TeamGenerator.h"
#include <random>
using namespace std;

// Class to create random teams with categories
class RandomCategoricalTeamGenerator : public TeamGenerator
{
public:
    vector<Team> createTeams(int numTeams);
};

#endif // RANDOMCATEGORICALTEAMGENERATOR_H