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
                try
                {
                    // Trim whitespace
                    weight_str.erase(0, weight_str.find_first_not_of(" \t\r\n"));
                    weight_str.erase(weight_str.find_last_not_of(" \t\r\n") + 1);

                    // Handle empty string
                    if (weight_str.empty())
                    {
                        weights.push_back(0.0);
                        continue;
                    }

                    // Manual conversion
                    double val = 0.0;
                    double fractional = 0.0;
                    double div = 1.0;
                    bool decimal_point = false;
                    bool negative = false;

                    for (char c : weight_str)
                    {
                        if (c == '-' && val == 0.0 && !decimal_point && !negative)
                        {
                            negative = true;
                        }
                        else if (c == '.' || c == ',')
                        {
                            decimal_point = true;
                        }
                        else if (c >= '0' && c <= '9')
                        {
                            if (decimal_point)
                            {
                                div *= 10.0;
                                fractional = fractional * 10.0 + (c - '0');
                            }
                            else
                            {
                                val = val * 10.0 + (c - '0');
                            }
                        }
                    }

                    double result = val + (fractional / div);
                    if (negative)
                        result = -result;
                    weights.push_back(result);
                }
                catch (...)
                {
                    // Fallback to 0.0 if conversion fails
                    weights.push_back(0.0);
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