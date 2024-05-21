#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <random>
#include <queue>
#include <map>

using namespace std;

// Helper function to draw a line
void drawLine(int length = 60);

// Helper function to print a message with spacing
void printMessage(const string& message, int spacing = 2);

// Class to represent a Person
class Person {
protected:
    string name;
    double score;
public:
    Person(const string& n, double s);
    string getName() const;
    double getScore() const;
};

// Function to sort Persons by name
bool sortByName(const Person& s1, const Person& s2);

// Class to represent a team
class Team {
protected:
    vector<Person> members;
public:
    vector<Person> getmembers() { return members; }
    void addPerson(const Person& Person);
    void printTeamWithWeights() const;
    void printTeamWithoutWeights() const;
    void printTeamWithoutWeights(ostream& os) const;
};

class Generator {
protected:
    vector<Person> Persons;
    void virtual readPersonsFromFile(const string& filename, const vector<int>& categoryIndices, const vector<double>& weights) = 0;
};

// Class to create balanced teams
class TeamGenerator : public Generator {
protected:
    vector<Team> teams;
    vector<string> headers;
    vector<int> categoryIndices;
public:
    vector<string> getheaders() { return headers; }
    void readPersonsFromFile(const string& filename, const vector<int>& categoryIndices, const vector<double>& weights);
    vector<Team> createTeams(int numTeams);
    void printTeamsWithWeights(const vector<Team>& teams);
    void printTeamsWithoutWeights(const vector<Team>& teams);
    void searchTeamsByCategory(int categoryIndex, const string& filename);
    void displayCategories();
};

// Class to create random teams
class RandomTeamGenerator : public Generator {
public:
    void readPersonsFromFile(const string& filename, const vector<int>& categoryIndices = {}, const vector<double>& weights = {});
    vector<Team> createRandomTeams(int numTeams);
};

// Class to create random teams with categories
class RandomCategoricalTeamGenerator : public TeamGenerator {
public:
    vector<Team> createTeams(int numTeams) {
        // Sort Persons based on their scores in descending order
        sort(Persons.begin(), Persons.end(), [](const Person& s1, const Person& s2) {
            return s1.getScore() > s2.getScore();
        });

        teams.clear();
        teams.resize(numTeams);

        // Assign each Person to a random team in the order of their scores
        random_device rd;
        mt19937 gen(rd());
        for (const auto& Person : Persons) {
            uniform_int_distribution<> dis(0, numTeams - 1);
            int teamIndex = dis(gen);
            teams[teamIndex].addPerson(Person);
        }

        return teams;
    }
};

// Helper function to draw a line
void drawLine(int length) {
    cout << setfill('-') << setw(length) << "-" << setfill(' ') << endl;
}

// Helper function to print a message with spacing
void printMessage(const string& message, int spacing) {
    cout << setw(spacing) << "" << message << endl;
}

// Class to represent a Person
Person::Person(const string& n, double s) : name(n), score(s) {}

string Person::getName() const {
    return name;
}

double Person::getScore() const {
    return score;
}

// Function to sort Persons by name
bool sortByName(const Person& s1, const Person& s2) {
    return s1.getName() < s2.getName();
}

// Class to represent a team
void Team::addPerson(const Person& Person) {
    members.push_back(Person);
}

void Team::printTeamWithWeights() const {
    for (const auto& member : members) {
        cout << member.getName() << " (" << fixed << setprecision(2) << member.getScore() << "), ";
    }
    cout << endl;
}

void Team::printTeamWithoutWeights() const {
    vector<Person> sortedMembers = members;
    sort(sortedMembers.begin(), sortedMembers.end(), sortByName);

    for (const auto& member : sortedMembers) {
        cout << member.getName() << ", ";
    }
    cout << endl;
}

void Team::printTeamWithoutWeights(ostream& os) const {
    vector<Person> sortedMembers = members;
    sort(sortedMembers.begin(), sortedMembers.end(), sortByName);

    for (const auto& member : sortedMembers) {
        os << member.getName() << ", ";
    }
}

// Class to create balanced teams
void TeamGenerator::readPersonsFromFile(const string& filename, const vector<int>& categoryIndices, const vector<double>& weights) {
    string line;
    ifstream file(filename);

    //if file doesn't exist
     if (!file.is_open()) {
        cout << "Error: File \"" << filename << "\" doesn't exist." << endl;
        exit(0);
        return;
    }
    //if file is empty
    if (file.peek() == ifstream::traits_type::eof()) {
        cout << "Error: File \"" << filename << "\" is empty." << endl;
        exit(0);
        return;
    }

    // Get the number of categories from the header line
    getline(file, line);
    istringstream headerSS(line);
    string header;
    headers.clear(); // Clear the existing headers
    while (getline(headerSS, header, ',')) {
        headers.push_back(header);
    }
    int numCategories = headers.size() - 1; // -1 to exclude the "Name" header

    // Process the data lines
    Persons.clear(); // Clear the existing Persons
    while (getline(file, line)) {
        istringstream ss(line);
        string name;
        getline(ss, name, ',');

        vector<double> scores(numCategories, 0.0); // Initialize all scores to 0.0
        double weightedScore = 0.0;
        for (int i = 0; i < numCategories; ++i) {
            string scoreStr;
            getline(ss, scoreStr, ',');
            double score = stod(scoreStr);
            scores[i] = score;

            // Check if the current category is selected
            auto categoryIndex = find(categoryIndices.begin(), categoryIndices.end(), i);
            if (categoryIndex != categoryIndices.end()) {
                int weightIndex = categoryIndex - categoryIndices.begin();
                weightedScore += score * weights[weightIndex];
            }
        }

        Persons.push_back(Person(name, weightedScore));
    }

    this->categoryIndices = categoryIndices; // Store the category indices
}

// Custom comparator for the priority queue
struct PersonComparator {
    bool operator()(const Person& s1, const Person& s2) {
        return s1.getScore() < s2.getScore();
    }
};

vector<Team> TeamGenerator::createTeams(int numTeams) {
    // A priority queue to store Persons sorted by their scores in descending order
    priority_queue<Person, vector<Person>, PersonComparator> PersonPriorityQueue(Persons.begin(), Persons.end());

    teams.clear();
    teams.resize(numTeams);

    int currentTeam = 0;
    bool forward = true;

    while (!PersonPriorityQueue.empty()) {
        teams[currentTeam].addPerson(PersonPriorityQueue.top());
        PersonPriorityQueue.pop();

        // Update the currentTeam in a zig-zag pattern
        if (forward) {
            currentTeam++;
            if (currentTeam == numTeams) {
                forward = false;
                currentTeam = numTeams - 1;
            }
        } else {
            currentTeam--;
            if (currentTeam == -1) {
                forward = true;
                currentTeam = 0;
            }
        }
    }

    return teams;
}

void TeamGenerator::printTeamsWithWeights(const vector<Team>& teams) {
    for (int team = 0; team < teams.size(); team++) {
        cout << "Team " << team + 1 << ": ";
        teams[team].printTeamWithWeights();
        cout << endl;
    }
}

void TeamGenerator::printTeamsWithoutWeights(const vector<Team>& teams) {
    int subChoice;
    do {
    cout << "Sub-Menu:\n";
    cout << "1. Print teams to console\n";
    cout << "2. Write teams to a file\n";
    cout << "0. Return to previous menu\n";
    cout << "Enter your choice: ";
    cin >> subChoice;
    if (subChoice == 1) {
            for (int team = 0; team < teams.size(); ++team) {
                cout << "Team " << team + 1 << ": ";
                teams[team].printTeamWithoutWeights();
                cout << endl;
            }
        } else if (subChoice == 2) {
            string filename;
            cout << "Enter the filename: ";
            cin >> filename;

            ofstream file(filename);
            if (file.is_open()) {
                for (int team = 0; team < teams.size(); ++team) {
                    file << "Team " << team + 1 << ": ";
                    teams[team].printTeamWithoutWeights(file);
                    file << endl;
                }
                file.close();
                cout << "Teams written to " << filename << " successfully!" << endl;
            } else {
                cout << "Unable to open the file!" << endl;
            }
        }
    } while (subChoice != 0);
}

void TeamGenerator::searchTeamsByCategory(int categoryIndex, const string& filename) {
    // Check if the selected category was used to create the teams
    bool categoryUsed = false;
    for (int index : categoryIndices) {
        if (index == categoryIndex) {
            categoryUsed = true;
            break;
        }
    }

    if (!categoryUsed) {
        cout << "The selected category was not used to create the teams. Please choose another category." << endl;
        return;
    }

    // Read the scores from the file
    map<string, double> PersonScores;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        getline(file, line); // Skip the header row

        while (getline(file, line)) {
            istringstream ss(line);
            string name;
            vector<double> scores;
            getline(ss, name, ',');
            string scoreStr;
            while (getline(ss, scoreStr, ',')) {
                scores.push_back(stod(scoreStr));
            }
            PersonScores[name] = scores[categoryIndex + 1]; // Assuming 0-based category index
        }
        file.close();
    } else {
        cout << "Unable to open the file!" << endl;
        return;
    }

    // Create a map to store teams and their total scores in the selected category
    map<double, vector<int>, greater<double>> teamScores;
    for (int team = 0; team < teams.size(); ++team) {
        double totalScore = 0.0;
        for (const auto& Person : teams[team].getmembers()) {
            totalScore += PersonScores[Person.getName()];
        }
        teamScores[totalScore].push_back(team);
    }

    cout << "Teams sorted by total score in " << headers[categoryIndex + 1] << " category (highest to lowest):" << endl;
    int rank = 1;
    for (const auto& teamScore : teamScores) {
        // Sort the teams with the same score using bubble sort
        vector<int> sortedTeams = teamScore.second;
        for (int i = 0; i < sortedTeams.size() - 1; ++i) {
            for (int j = 0; j < sortedTeams.size() - i - 1; ++j) {
                if (teams[sortedTeams[j]].getmembers().size() < teams[sortedTeams[j + 1]].getmembers().size()) {
                    swap(sortedTeams[j], sortedTeams[j + 1]);
                }
            }
        }

        for (int teamIndex : sortedTeams) {
            cout << "Rank " << rank << ": Team " << teamIndex + 1 << " (Total Score: " << teamScore.first << "): ";
            teams[teamIndex].printTeamWithoutWeights();
            cout << endl;
            ++rank;
        }
    }
}

void TeamGenerator::displayCategories() {
    if (headers.size() <= 1) {
        cout << "No categories available." << endl;
        return;
    }

    cout << "Categories:" << endl;
    for (int i = 1; i < headers.size(); ++i) {
        cout << i - 1 << " - " << headers[i] << endl;
    }
}
void RandomTeamGenerator::readPersonsFromFile(const string& filename, const vector<int>& categoryIndices, const vector<double>& weight){
    string line;
    ifstream file(filename);

    // Skip the header line
    getline(file, line);

    // Process the data lines
    Persons.clear(); // Clear the existing Persons
    while (getline(file, line)) {
        istringstream ss(line);
        string name;
        getline(ss, name, ',');
        Persons.push_back(Person(name, 0.0)); // Scores are not used for random teams
    }
}

vector<Team> RandomTeamGenerator::createRandomTeams(int numTeams) {
    random_device rd;
    mt19937 gen(rd());

    vector<Team> teams(numTeams);
    int PersonIndex = 0;
    for (const auto& Person : Persons) {
        int teamIndex = PersonIndex % numTeams;
        teams[teamIndex].addPerson(Person);
        PersonIndex++;
    }

    return teams;
}

int main() {
    drawLine();
    printMessage("Welcome to The Team Generator Application!", 4);
    drawLine();

    string filename;
    cout << "Enter the filename (e.g., data.csv): ";
    cin >> filename;

    TeamGenerator teamGenerator;
    teamGenerator.readPersonsFromFile(filename, {}, {});  // Read the file and populate headers

    int choice;
    do {
        cout << "Main Menu:\n";
        cout << "1. Generate teams randomly\n";
        cout << "2. Generate teams using categories\n";
        cout << "3. Generate teams randomly with categories\n";
        cout << "4. Display categories\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            int numTeams;
            cout << "Enter the number of teams: ";
            cin >> numTeams;

            RandomTeamGenerator randomTeamGenerator;
            randomTeamGenerator.readPersonsFromFile(filename);
            vector<Team> teams = randomTeamGenerator.createRandomTeams(numTeams);

            drawLine();
            printMessage("The Random Team Generator");
            drawLine();

            for (int team = 0; team < teams.size(); ++team) {
                cout << "Team " << team + 1 << ": ";
                teams[team].printTeamWithoutWeights();
                cout << endl;
            }
        } else if (choice == 2) {
            int numTeams;
            cout << "Enter the number of teams: ";
            cin >> numTeams;

            int numCategories;
            cout << "Enter the number of categories: ";
            cin >> numCategories;

            vector<int> categoryIndices(numCategories);
            cout << "Enter the category indices (0-based, separated by spaces): ";
            for (int i = 0; i < numCategories; ++i) {
                cin >> categoryIndices[i];
            }

            vector<double> weights(numCategories);
            double weightSum = 0.0;
            cout << "Enter the weights for the selected categories (separated by spaces): ";
            for (int i = 0; i < numCategories; ++i) {
                cin >> weights[i];
                weightSum += weights[i];
            }

            if (abs(weightSum - 1.0) > 1e-9) {
                cout << "Error: The sum of weights should be 1.0" << endl;
                continue;
            }

            teamGenerator.readPersonsFromFile(filename, categoryIndices, weights);
            vector<Team> teams = teamGenerator.createTeams(numTeams);

            drawLine();
            printMessage("The Ultimate Team Generator");
            drawLine();

            int subChoice;
            do {
                cout << "Sub-Menu:\n";
                cout << "1. Print Teams with weights\n";
                cout << "2. Print Teams without weights (in sorted order)\n";
                cout << "3. Filter Teams on basis of category\n";
                cout << "0. Return to Main Menu\n";
                cout << "Enter your choice: ";
                cin >> subChoice;

                if (subChoice == 1) {
                    teamGenerator.printTeamsWithWeights(teams);
                } else if (subChoice == 2) {
                    teamGenerator.printTeamsWithoutWeights(teams);
                } else if (subChoice == 3) {
                    int categoryIndex;
                    cout << "Enter the category index (0-based): ";
                    cin >> categoryIndex;

                    if (categoryIndex < 0 || categoryIndex >= teamGenerator.getheaders().size() - 1) {
                        cout << "Invalid category index!" << endl;
                    } else {
                        teamGenerator.searchTeamsByCategory(categoryIndex, filename);
                    }
                }
            } while (subChoice != 0);
        } else if (choice == 4) {
            teamGenerator.displayCategories();
        } else if (choice == 3) {
            int numTeams;
            cout << "Enter the number of teams: ";
            cin >> numTeams;

            int numCategories;
            cout << "Enter the number of categories: ";
            cin >> numCategories;

            vector<int> categoryIndices(numCategories);
            cout << "Enter the category indices (0-based, separated by spaces): ";
            for (int i = 0; i < numCategories; ++i) {
                cin >> categoryIndices[i];
            }

            vector<double> weights(numCategories);
            double weightSum = 0.0;
            cout << "Enter the weights for the selected categories (separated by spaces): ";
            for (int i = 0; i < numCategories; ++i) {
                cin >> weights[i];
                weightSum += weights[i];
            }

            if (abs(weightSum - 1.0) > 1e-9) {
                cout << "Error: The sum of weights should be 1.0" << endl;
                continue;
            }

            RandomCategoricalTeamGenerator randomCategoricalTeamGenerator;
            randomCategoricalTeamGenerator.readPersonsFromFile(filename, categoryIndices, weights);
            vector<Team> teams = randomCategoricalTeamGenerator.createTeams(numTeams);

            drawLine();
            printMessage("The Random Team Generator with Categories");
            drawLine();

            int subChoice;
            do {
                cout << "Sub-Menu:\n";
                cout << "1. Print Teams with weights\n";
                cout << "2. Print Teams without weights (in sorted order)\n";
                cout << "3. Filter Teams on basis of category\n";
                cout << "0. Return to Main Menu\n";
                cout << "Enter your choice: ";
                cin >> subChoice;

                if (subChoice == 1) {
                    randomCategoricalTeamGenerator.printTeamsWithWeights(teams);
                } else if (subChoice == 2) {
                    randomCategoricalTeamGenerator.printTeamsWithoutWeights(teams);
                } else if (subChoice == 3) {
                    int categoryIndex;
                    cout << "Enter the category index (0-based): ";
                    cin >> categoryIndex;

                    if (categoryIndex < 0 || categoryIndex >= randomCategoricalTeamGenerator.getheaders().size() - 1) {
                        cout << "Invalid category index!" << endl;
                    } else {
                        randomCategoricalTeamGenerator.searchTeamsByCategory(categoryIndex, filename);
                    }
                }
            } while (subChoice != 0);
        }
    } while (choice != 0);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    return 0;
}
