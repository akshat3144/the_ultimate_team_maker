#include "../include/Team.h"
#include <algorithm>
#include <iomanip>
using namespace std;

vector<Person> Team::getmembers()
{
    return members;
}

// Class to represent a team
void Team::addPerson(const Person &Person)
{
    members.push_back(Person);
}

void Team::printTeamWithWeights() const
{
    for (const auto &member : members)
    {
        cout << member.getName() << " (" << fixed << setprecision(2) << member.getScore() << "), ";
    }
    cout << endl;
}

void Team::printTeamWithoutWeights() const
{
    vector<Person> sortedMembers = members;
    sort(sortedMembers.begin(), sortedMembers.end(), sortByName);

    for (const auto &member : sortedMembers)
    {
        cout << member.getName() << ", ";
    }
    cout << endl;
}

void Team::printTeamWithoutWeights(ostream &os) const
{
    vector<Person> sortedMembers = members;
    sort(sortedMembers.begin(), sortedMembers.end(), sortByName);

    for (const auto &member : sortedMembers)
    {
        os << member.getName() << ", ";
    }
}