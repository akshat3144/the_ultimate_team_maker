#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "../include/Person.h"
#include "../include/Team.h"
#include "../include/TeamGenerator.h"
#include "../include/RandomTeamGenerator.h"
#include "../include/RandomCategoricalTeamGenerator.h"
using namespace std;

void outputTeamsAsJson(const vector<Team> &teams)
{
    cout << "[";
    for (size_t i = 0; i < teams.size(); ++i)
    {
        const Team &team = teams[i];
        vector<Person> members = team.getmembers();

        if (i > 0)
            cout << ",";
        cout << "{\"team_number\":" << (i + 1) << ",\"members\":[";

        for (size_t j = 0; j < members.size(); ++j)
        {
            if (j > 0)
                cout << ",";
            cout << "\"" << members[j].getName() << "\"";
        }
        cout << "]}";
    }
    cout << "]";
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cerr << "Usage: " << argv[0] << " <csv_file_path> <generation_type> <num_teams> [<cat_indices> <weights>]" << endl;
        return 1;
    }

    string filename = argv[1];
    string generation_type = argv[2];
    int num_teams = stoi(argv[3]);

    try
    {
        if (generation_type == "random")
        {
            RandomTeamGenerator generator;
            generator.readPersonsFromFile(filename);
            vector<Team> teams = generator.createRandomTeams(num_teams);
            outputTeamsAsJson(teams);
        }
        else if (generation_type == "categorical" || generation_type == "random_categorical")
        {
            if (argc < 6)
            {
                cerr << "For categorical generation, category indices and weights are required" << endl;
                return 1;
            }

            string cat_indices_str = argv[4];
            string weights_str = argv[5];

            // Parse category indices
            istringstream cat_ss(cat_indices_str);
            string index_str;
            vector<int> categoryIndices;

            while (getline(cat_ss, index_str, ','))
            {
                categoryIndices.push_back(stoi(index_str));
            }

            // Parse weights
            istringstream weights_ss(weights_str);
            string weight_str;
            vector<double> weights;

            while (getline(weights_ss, weight_str, ','))
            {
                // Add robust error handling for stod conversion
                try
                {
                    // Replace commas with periods to handle different locale formats
                    size_t pos = 0;
                    while ((pos = weight_str.find(",", pos)) != string::npos)
                    {
                        weight_str.replace(pos, 1, ".");
                        pos += 1;
                    }

                    // Trim any whitespace
                    weight_str.erase(0, weight_str.find_first_not_of(" \t"));
                    weight_str.erase(weight_str.find_last_not_of(" \t") + 1);

                    // Validate the string before conversion
                    if (weight_str.empty())
                    {
                        cerr << "Error: Empty weight value" << endl;
                        return 1;
                    }

                    // Try to convert to double
                    double weight = stod(weight_str);
                    weights.push_back(weight);
                }
                catch (const exception &e)
                {
                    cerr << "Error converting weight '" << weight_str << "' to double: " << e.what() << endl;
                    return 1;
                }
            }

            if (generation_type == "categorical")
            {
                TeamGenerator generator;
                generator.readPersonsFromFile(filename, categoryIndices, weights);
                vector<Team> teams = generator.createTeams(num_teams);
                outputTeamsAsJson(teams);
            }
            else
            {
                RandomCategoricalTeamGenerator generator;
                generator.readPersonsFromFile(filename, categoryIndices, weights);
                vector<Team> teams = generator.createTeams(num_teams);
                outputTeamsAsJson(teams);
            }
        }
        else
        {
            cerr << "Invalid generation type. Must be 'random', 'categorical', or 'random_categorical'" << endl;
            return 1;
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}