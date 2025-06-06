#include "../include/RandomCategoricalTeamGenerator.h"
#include <random>
#include <algorithm>
using namespace std;

vector<Team> RandomCategoricalTeamGenerator::createTeams(int numTeams)
{
    // Sort Persons based on their scores in descending order
    sort(Persons.begin(), Persons.end(), [](const Person &s1, const Person &s2)
              { return s1.getScore() > s2.getScore(); });

    teams.clear();
    teams.resize(numTeams);

    // Assign each Person to a random team in the order of their scores
    random_device rd;
    mt19937 gen(rd());
    for (const auto &Person : Persons)
    {
        uniform_int_distribution<> dis(0, numTeams - 1);
        int teamIndex = dis(gen);
        teams[teamIndex].addPerson(Person);
    }

    return teams;
}