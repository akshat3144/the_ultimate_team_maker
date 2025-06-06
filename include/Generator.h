#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <string>
#include "Person.h"
using namespace std;

class Generator
{
protected:
    vector<Person> Persons;
    void virtual readPersonsFromFile(const string &filename, const vector<int> &categoryIndices, const vector<double> &weights) = 0;
};

#endif // GENERATOR_H