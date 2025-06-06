#ifndef TEAM_H
#define TEAM_H

#include <vector>
#include <iostream>
#include "Person.h"
using namespace std;

// Class to represent a team
class Team
{
protected:
    vector<Person> members;

public:
    vector<Person> getmembers();
    void addPerson(const Person &Person);
    void printTeamWithWeights() const;
    void printTeamWithoutWeights() const;
    void printTeamWithoutWeights(ostream &os) const;
};

#endif // TEAM_H