#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <csv_file_path>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Could not open file \"" << filename << "\"" << endl;
        return 1;
    }

    string line;
    if (getline(file, line))
    {
        // Remove any carriage returns or line feeds
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        istringstream headerSS(line);
        string header;
        vector<string> headers;

        while (getline(headerSS, header, ','))
        {
            // Trim any whitespace
            header.erase(0, header.find_first_not_of(" \t"));
            header.erase(header.find_last_not_of(" \t") + 1);

            headers.push_back(header);
        }

        // Output as JSON array
        cout << "[";
        for (size_t i = 0; i < headers.size(); ++i)
        {
            if (i > 0)
            {
                cout << ",";
            }
            cout << "\"" << headers[i] << "\"";
        }
        cout << "]";
    }
    else
    {
        cerr << "Error: File is empty" << endl;
        return 1;
    }

    return 0;
}