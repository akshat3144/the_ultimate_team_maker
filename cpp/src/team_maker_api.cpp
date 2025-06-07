#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
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

// New function to search teams by category and output results as JSON
void searchTeamsByCategoryJson(const vector<Team> &teams, int categoryIndex, const string &filename, const vector<int> &categoryIndices)
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
        cout << "{\"error\":\"The selected category was not used to create the teams.\"}";
        return;
    }

    // Read the headers to get category name
    string categoryName = "";
    ifstream headerFile(filename);
    if (headerFile.is_open())
    {
        string line;
        if (getline(headerFile, line))
        {
            istringstream headerSS(line);
            string header;
            vector<string> headers;
            while (getline(headerSS, header, ','))
            {
                headers.push_back(header);
            }
            if (categoryIndex + 1 < headers.size())
            {
                categoryName = headers[categoryIndex + 1];
            }
        }
        headerFile.close();
    }

    // Read the scores from the file
    map<string, double> personScores;
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

            // Read each score
            int scoreIndex = 0;
            string scoreStr;
            while (getline(ss, scoreStr, ','))
            {
                try
                {
                    double score = stod(scoreStr);
                    if (scoreIndex == categoryIndex)
                    {
                        personScores[name] = score;
                    }
                }
                catch (...)
                {
                    // Handle conversion error
                    if (scoreIndex == categoryIndex)
                    {
                        personScores[name] = 0.0;
                    }
                }
                scoreIndex++;
            }
        }
        file.close();
    }
    else
    {
        cout << "{\"error\":\"Unable to open the file!\"}";
        return;
    }

    // Create a map to store teams and their total scores in the selected category
    vector<pair<int, double>> teamScores;
    for (size_t team = 0; team < teams.size(); ++team)
    {
        double totalScore = 0.0;
        for (const auto &person : teams[team].getmembers())
        {
            totalScore += personScores[person.getName()];
        }
        teamScores.push_back({team, totalScore});
    }

    // Sort by score in descending order
    sort(teamScores.begin(), teamScores.end(),
         [](const pair<int, double> &a, const pair<int, double> &b)
         {
             return a.second > b.second;
         });

    // Output as JSON
    cout << "{\"category_name\":\"" << categoryName << "\",\"results\":[";
    for (size_t i = 0; i < teamScores.size(); ++i)
    {
        int teamIndex = teamScores[i].first;
        double score = teamScores[i].second;

        if (i > 0)
            cout << ",";

        cout << "{\"rank\":" << (i + 1)
             << ",\"team_number\":" << (teamIndex + 1)
             << ",\"score\":" << score
             << ",\"members\":[";

        const auto &members = teams[teamIndex].getmembers();
        for (size_t j = 0; j < members.size(); ++j)
        {
            if (j > 0)
                cout << ",";
            cout << "{\"name\":\"" << members[j].getName()
                 << "\",\"individual_score\":" << personScores[members[j].getName()] << "}";
        }
        cout << "]}";
    }
    cout << "]}";
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <csv_file_path> <command> [<additional_args>...]" << endl;
        return 1;
    }

    string filename = argv[1];
    string command = argv[2];

    try
    {
        if (command == "generate")
        {
            if (argc < 5)
            {
                cerr << "For generate: " << argv[0] << " <csv_file_path> generate <generation_type> <num_teams> [<cat_indices> <weights>]" << endl;
                return 1;
            }

            string generation_type = argv[3];
            int num_teams = stoi(argv[4]);

            if (generation_type == "random")
            {
                RandomTeamGenerator generator;
                generator.readPersonsFromFile(filename);
                vector<Team> teams = generator.createRandomTeams(num_teams);
                outputTeamsAsJson(teams);
            }
            else if (generation_type == "categorical" || generation_type == "random_categorical")
            {
                if (argc < 7)
                {
                    cerr << "For categorical generation, category indices and weights are required" << endl;
                    return 1;
                }

                string cat_indices_str = argv[5];
                string weights_str = argv[6];

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
        else if (command == "search")
        {
            if (argc < 7)
            {
                cerr << "For search: " << argv[0] << " <csv_file_path> search <generation_type> <num_teams> <category_index> <cat_indices> <weights>" << endl;
                return 1;
            }

            string generation_type = argv[3];
            int num_teams = stoi(argv[4]);
            int category_index = stoi(argv[5]);
            string cat_indices_str = argv[6];
            string weights_str = argv[7];

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
                weights.push_back(stod(weight_str));
            }

            vector<Team> teams;

            if (generation_type == "random")
            {
                RandomTeamGenerator generator;
                generator.readPersonsFromFile(filename);
                teams = generator.createRandomTeams(num_teams);
            }
            else if (generation_type == "categorical")
            {
                TeamGenerator generator;
                generator.readPersonsFromFile(filename, categoryIndices, weights);
                teams = generator.createTeams(num_teams);
            }
            else if (generation_type == "random_categorical")
            {
                RandomCategoricalTeamGenerator generator;
                generator.readPersonsFromFile(filename, categoryIndices, weights);
                teams = generator.createTeams(num_teams);
            }
            else
            {
                cerr << "Invalid generation type. Must be 'random', 'categorical', or 'random_categorical'" << endl;
                return 1;
            }

            searchTeamsByCategoryJson(teams, category_index, filename, categoryIndices);
        }
        else
        {
            cerr << "Unknown command. Must be 'generate' or 'search'" << endl;
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