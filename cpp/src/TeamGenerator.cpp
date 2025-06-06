#include "../include/TeamGenerator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <map>
using namespace std;

vector<string> TeamGenerator::getheaders()
{
    return headers;
}

// Class to create balanced teams
void TeamGenerator::readPersonsFromFile(const string &filename, const vector<int> &categoryIndices, const vector<double> &weights)
{
    string line;
    ifstream file(filename);

    // if file doesn't exist
    if (!file.is_open())
    {
        cout << "Error: File \"" << filename << "\" doesn't exist." << endl;
        exit(0);
        return;
    }
    // if file is empty
    if (file.peek() == ifstream::traits_type::eof())
    {
        cout << "Error: File \"" << filename << "\" is empty." << endl;
        exit(0);
        return;
    }

    // Get the number of categories from the header line
    getline(file, line);
    istringstream headerSS(line);
    string header;
    headers.clear(); // Clear the existing headers
    while (getline(headerSS, header, ','))
    {
        headers.push_back(header);
    }
    int numCategories = headers.size() - 1; // -1 to exclude the "Name" header

    // Process the data lines
    Persons.clear(); // Clear the existing Persons
    while (getline(file, line))
    {
        istringstream ss(line);
        string name;
        getline(ss, name, ',');

        vector<double> scores(numCategories, 0.0); // Initialize all scores to 0.0
        double weightedScore = 0.0;
        for (int i = 0; i < numCategories; ++i)
        {
            string scoreStr;
            getline(ss, scoreStr, ',');
            double score = stod(scoreStr);
            scores[i] = score;

            // Check if the current category is selected
            auto categoryIndex = find(categoryIndices.begin(), categoryIndices.end(), i);
            if (categoryIndex != categoryIndices.end())
            {
                int weightIndex = categoryIndex - categoryIndices.begin();
                weightedScore += score * weights[weightIndex];
            }
        }

        Persons.push_back(Person(name, weightedScore));
    }

    this->categoryIndices = categoryIndices; // Store the category indices
}

// Custom comparator for the priority queue
struct PersonComparator
{
    bool operator()(const Person &s1, const Person &s2)
    {
        return s1.getScore() < s2.getScore();
    }
};

vector<Team> TeamGenerator::createTeams(int numTeams)
{
    // A priority queue to store Persons sorted by their scores in descending order
    priority_queue<Person, vector<Person>, PersonComparator> PersonPriorityQueue(Persons.begin(), Persons.end());

    teams.clear();
    teams.resize(numTeams);

    int currentTeam = 0;
    bool forward = true;

    while (!PersonPriorityQueue.empty())
    {
        teams[currentTeam].addPerson(PersonPriorityQueue.top());
        PersonPriorityQueue.pop();

        // Update the currentTeam in a zig-zag pattern
        if (forward)
        {
            currentTeam++;
            if (currentTeam == numTeams)
            {
                forward = false;
                currentTeam = numTeams - 1;
            }
        }
        else
        {
            currentTeam--;
            if (currentTeam == -1)
            {
                forward = true;
                currentTeam = 0;
            }
        }
    }

    return teams;
}

void TeamGenerator::printTeamsWithWeights(const vector<Team> &teams)
{
    for (int team = 0; team < teams.size(); team++)
    {
        cout << "Team " << team + 1 << ": ";
        teams[team].printTeamWithWeights();
        cout << endl;
    }
}

void TeamGenerator::printTeamsWithoutWeights(const vector<Team> &teams)
{
    int subChoice;
    do
    {
        cout << "Sub-Menu:\n";
        cout << "1. Print teams to console\n";
        cout << "2. Write teams to a file\n";
        cout << "0. Return to previous menu\n";
        cout << "Enter your choice: ";
        cin >> subChoice;
        if (subChoice == 1)
        {
            for (int team = 0; team < teams.size(); ++team)
            {
                cout << "Team " << team + 1 << ": ";
                teams[team].printTeamWithoutWeights();
                cout << endl;
            }
        }
        else if (subChoice == 2)
        {
            string filename;
            cout << "Enter the filename: ";
            cin >> filename;

            ofstream file(filename);
            if (file.is_open())
            {
                for (int team = 0; team < teams.size(); ++team)
                {
                    file << "Team " << team + 1 << ": ";
                    teams[team].printTeamWithoutWeights(file);
                    file << endl;
                }
                file.close();
                cout << "Teams written to " << filename << " successfully!" << endl;
            }
            else
            {
                cout << "Unable to open the file!" << endl;
            }
        }
    } while (subChoice != 0);
}

void TeamGenerator::searchTeamsByCategory(int categoryIndex, const string &filename)
{
    // Check if the selected category was used to create the teams
    bool categoryUsed = false;
    for (int index : categoryIndices)
    {
        if (index == categoryIndex)
        {
            categoryUsed = true;
            break;
        }
    }

    if (!categoryUsed)
    {
        cout << "The selected category was not used to create the teams. Please choose another category." << endl;
        return;
    }

    // Read the scores from the file
    map<string, double> PersonScores;
    ifstream file(filename);
    if (file.is_open())
    {
        string line;
        getline(file, line); // Skip the header row

        while (getline(file, line))
        {
            istringstream ss(line);
            string name;
            vector<double> scores;
            getline(ss, name, ',');
            string scoreStr;
            while (getline(ss, scoreStr, ','))
            {
                scores.push_back(stod(scoreStr));
            }
            PersonScores[name] = scores[categoryIndex + 1]; // Assuming 0-based category index
        }
        file.close();
    }
    else
    {
        cout << "Unable to open the file!" << endl;
        return;
    }

    // Create a map to store teams and their total scores in the selected category
    map<double, vector<int>, greater<double>> teamScores;
    for (int team = 0; team < teams.size(); ++team)
    {
        double totalScore = 0.0;
        for (const auto &Person : teams[team].getmembers())
        {
            totalScore += PersonScores[Person.getName()];
        }
        teamScores[totalScore].push_back(team);
    }

    cout << "Teams sorted by total score in " << headers[categoryIndex + 1] << " category (highest to lowest):" << endl;
    int rank = 1;
    for (const auto &teamScore : teamScores)
    {
        // Sort the teams with the same score using bubble sort
        vector<int> sortedTeams = teamScore.second;
        for (int i = 0; i < sortedTeams.size() - 1; ++i)
        {
            for (int j = 0; j < sortedTeams.size() - i - 1; ++j)
            {
                if (teams[sortedTeams[j]].getmembers().size() < teams[sortedTeams[j + 1]].getmembers().size())
                {
                    swap(sortedTeams[j], sortedTeams[j + 1]);
                }
            }
        }

        for (int teamIndex : sortedTeams)
        {
            cout << "Rank " << rank << ": Team " << teamIndex + 1 << " (Total Score: " << teamScore.first << "): ";
            teams[teamIndex].printTeamWithoutWeights();
            cout << endl;
            ++rank;
        }
    }
}

void TeamGenerator::displayCategories()
{
    if (headers.size() <= 1)
    {
        cout << "No categories available." << endl;
        return;
    }

    cout << "Categories:" << endl;
    for (int i = 1; i < headers.size(); ++i)
    {
        cout << i - 1 << " - " << headers[i] << endl;
    }
}