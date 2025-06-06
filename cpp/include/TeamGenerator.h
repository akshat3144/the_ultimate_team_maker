#ifndef TEAMGENERATOR_H
#define TEAMGENERATOR_H

#include "Generator.h"
#include "Team.h"
#include <vector>
#include <string>
using namespace std;

// Class to create balanced teams
class TeamGenerator : public Generator
{
protected:
    vector<Team> teams;
    vector<string> headers;
    vector<int> categoryIndices;

public:
    vector<string> getheaders();
    void readPersonsFromFile(const string &filename, const vector<int> &categoryIndices, const vector<double> &weights);
    vector<Team> createTeams(int numTeams);
    void printTeamsWithWeights(const vector<Team> &teams);
    void printTeamsWithoutWeights(const vector<Team> &teams);
    void searchTeamsByCategory(int categoryIndex, const string &filename);
    void displayCategories();
};

#endif // TEAMGENERATOR_H