#include <iostream>
#include <iomanip>
#include <cmath>
#include "../include/Person.h"
#include "../include/Team.h"
#include "../include/TeamGenerator.h"
#include "../include/RandomTeamGenerator.h"
#include "../include/RandomCategoricalTeamGenerator.h"
#include "../include/Utilities.h"

using namespace std;

int main()
{
    drawLine();
    printMessage("Welcome to The Team Generator Application!", 4);
    drawLine();

    string filename;
    cout << "Enter the filename (e.g., data.csv): ";
    cin >> filename;

    TeamGenerator teamGenerator;
    teamGenerator.readPersonsFromFile(filename, {}, {}); // Read the file and populate headers

    int choice;
    do
    {
        cout << "Main Menu:\n";
        cout << "1. Generate teams randomly\n";
        cout << "2. Generate teams using categories\n";
        cout << "3. Generate teams randomly with categories\n";
        cout << "4. Display categories\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int numTeams;
            cout << "Enter the number of teams: ";
            cin >> numTeams;

            RandomTeamGenerator randomTeamGenerator;
            randomTeamGenerator.readPersonsFromFile(filename);
            vector<Team> teams = randomTeamGenerator.createRandomTeams(numTeams);

            drawLine();
            printMessage("The Random Team Generator", 4);
            drawLine();

            for (int team = 0; team < teams.size(); ++team)
            {
                cout << "Team " << team + 1 << ": ";
                teams[team].printTeamWithoutWeights();
                cout << endl;
            }
        }
        else if (choice == 2)
        {
            int numTeams;
            cout << "Enter the number of teams: ";
            cin >> numTeams;

            int numCategories;
            cout << "Enter the number of categories: ";
            cin >> numCategories;

            vector<int> categoryIndices(numCategories);
            cout << "Enter the category indices (0-based, separated by spaces): ";
            for (int i = 0; i < numCategories; ++i)
            {
                cin >> categoryIndices[i];
            }

            vector<double> weights(numCategories);
            double weightSum = 0.0;
            cout << "Enter the weights for the selected categories (separated by spaces): ";
            for (int i = 0; i < numCategories; ++i)
            {
                cin >> weights[i];
                weightSum += weights[i];
            }

            if (abs(weightSum - 1.0) > 1e-9)
            {
                cout << "Error: The sum of weights should be 1.0" << endl;
                continue;
            }

            teamGenerator.readPersonsFromFile(filename, categoryIndices, weights);
            vector<Team> teams = teamGenerator.createTeams(numTeams);

            drawLine();
            printMessage("The Ultimate Team Generator", 4);
            drawLine();

            int subChoice;
            do
            {
                cout << "Sub-Menu:\n";
                cout << "1. Print Teams with weights\n";
                cout << "2. Print Teams without weights (in sorted order)\n";
                cout << "3. Filter Teams on basis of category\n";
                cout << "0. Return to Main Menu\n";
                cout << "Enter your choice: ";
                cin >> subChoice;

                if (subChoice == 1)
                {
                    teamGenerator.printTeamsWithWeights(teams);
                }
                else if (subChoice == 2)
                {
                    teamGenerator.printTeamsWithoutWeights(teams);
                }
                else if (subChoice == 3)
                {
                    int categoryIndex;
                    cout << "Enter the category index (0-based): ";
                    cin >> categoryIndex;

                    if (categoryIndex < 0 || categoryIndex >= teamGenerator.getheaders().size() - 1)
                    {
                        cout << "Invalid category index!" << endl;
                    }
                    else
                    {
                        teamGenerator.searchTeamsByCategory(categoryIndex, filename);
                    }
                }
            } while (subChoice != 0);
        }
        else if (choice == 4)
        {
            teamGenerator.displayCategories();
        }
        else if (choice == 3)
        {
            int numTeams;
            cout << "Enter the number of teams: ";
            cin >> numTeams;

            int numCategories;
            cout << "Enter the number of categories: ";
            cin >> numCategories;

            vector<int> categoryIndices(numCategories);
            cout << "Enter the category indices (0-based, separated by spaces): ";
            for (int i = 0; i < numCategories; ++i)
            {
                cin >> categoryIndices[i];
            }

            vector<double> weights(numCategories);
            double weightSum = 0.0;
            cout << "Enter the weights for the selected categories (separated by spaces): ";
            for (int i = 0; i < numCategories; ++i)
            {
                cin >> weights[i];
                weightSum += weights[i];
            }

            if (abs(weightSum - 1.0) > 1e-9)
            {
                cout << "Error: The sum of weights should be 1.0" << endl;
                continue;
            }

            RandomCategoricalTeamGenerator randomCategoricalTeamGenerator;
            randomCategoricalTeamGenerator.readPersonsFromFile(filename, categoryIndices, weights);
            vector<Team> teams = randomCategoricalTeamGenerator.createTeams(numTeams);

            drawLine();
            printMessage("The Random Team Generator with Categories", 4);
            drawLine();

            int subChoice;
            do
            {
                cout << "Sub-Menu:\n";
                cout << "1. Print Teams with weights\n";
                cout << "2. Print Teams without weights (in sorted order)\n";
                cout << "3. Filter Teams on basis of category\n";
                cout << "0. Return to Main Menu\n";
                cout << "Enter your choice: ";
                cin >> subChoice;

                if (subChoice == 1)
                {
                    randomCategoricalTeamGenerator.printTeamsWithWeights(teams);
                }
                else if (subChoice == 2)
                {
                    randomCategoricalTeamGenerator.printTeamsWithoutWeights(teams);
                }
                else if (subChoice == 3)
                {
                    int categoryIndex;
                    cout << "Enter the category index (0-based): ";
                    cin >> categoryIndex;

                    if (categoryIndex < 0 || categoryIndex >= randomCategoricalTeamGenerator.getheaders().size() - 1)
                    {
                        cout << "Invalid category index!" << endl;
                    }
                    else
                    {
                        randomCategoricalTeamGenerator.searchTeamsByCategory(categoryIndex, filename);
                    }
                }
            } while (subChoice != 0);
        }
    } while (choice != 0);
    return 0;
}