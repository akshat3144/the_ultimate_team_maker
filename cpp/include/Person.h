#ifndef PERSON_H
#define PERSON_H

#include <string>
using namespace std;

// Class to represent a Person
class Person
{
protected:
    string name;
    double score;

public:
    Person(const string &n, double s);
    string getName() const;
    double getScore() const;
};

// Function to sort Persons by name
bool sortByName(const Person &s1, const Person &s2);

#endif // PERSON_H