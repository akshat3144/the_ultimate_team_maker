#include "../include/Person.h"
using namespace std;

// Class to represent a Person
Person::Person(const string &n, double s) : name(n), score(s) {}

string Person::getName() const
{
    return name;
}

double Person::getScore() const
{
    return score;
}

// Function to sort Persons by name
bool sortByName(const Person &s1, const Person &s2)
{
    return s1.getName() < s2.getName();
}