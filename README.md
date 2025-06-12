# Team Maker

Team Maker is a versatile application for generating balanced teams based on various attributes or categories. It provides multiple interfaces including a command-line tool, a web-based GUI, and a RESTful API.

## Features

- **Multiple Team Generation Methods**:

  - **Random**: Creates teams by randomly distributing participants
  - **Categorical**: Creates balanced teams by considering selected categories and their weights
  - **Random Categorical**: Combines randomness with category-based balancing

- **Advanced Team Analysis**:

  - Search and filter teams based on specific categories
  - Display teams with or without weighted scores
  - Sort and rank teams by category scores

- **Multiple Interfaces**:
  - Command-line interface (CLI) for direct desktop usage
  - Web-based GUI for easy interaction through a browser
  - RESTful API for integration with other applications

## Project Structure

```
team_maker/
├── api/              # FastAPI implementation for the RESTful API
├── bin/              # Compiled executables
├── cpp/              # C++ source code
│   ├── include/      # Header files
│   └── src/          # Implementation files
├── frontend/         # Web interface files (HTML, CSS, JavaScript)
├── data.csv          # Sample data file
├── Dockerfile        # Docker configuration for containerization
└── requirements.txt  # Python dependencies
```

## Usage

### Command-line Interface (CLI)

1. Navigate to the project root directory
2. Run the executable:
   ```bash
   ./bin/team_maker.exe
   ```
3. Follow the interactive prompts to:
   - Provide a CSV file with participant data
   - Choose a team generation method
   - Configure categories and weights (if applicable)
   - Select output options

### Web Interface

1. Start the web server:
   ```bash
   cd api
   uvicorn main:app --reload
   ```
2. Open your browser and navigate to `http://localhost:8000`
3. Upload your CSV file with participant data
4. Select your desired team generation method
5. Configure categories and weights if needed
6. Generate and view your teams

### API Usage

The application provides a RESTful API for programmatic access:

- **Upload CSV**: `POST /upload-csv/`
- **Generate Teams**: `POST /generate-teams/`
- **Search Teams by Category**: `POST /search-teams-by-category/`

Example API request to generate teams:

```json
POST /generate-teams/
{
  "generation_type": "categorical",
  "num_teams": 4,
  "file_path": "/path/to/data.csv",
  "categories": [
    {
      "index": 0,
      "weight": 0.5,
      "name": "Coding"
    },
    {
      "index": 1,
      "weight": 0.5,
      "name": "Speaking"
    }
  ]
}
```

## Data Format

The application uses CSV files with the following structure:

- First row: Header with category names
- First column: Participant names
- Other columns: Numerical scores for each category

Example:

```csv
Name,Coding,Speaking,Sleeping,Fighting
Akshat,10.0,9.0,10.0,4.0
Rahul,9.0,8.0,9.0,8.5
Ravi,8.5,7.5,8.0,9.0
Priya,7.0,9.0,10.0,7.5
Sneha,8.0,8.0,9.0,8.0
```

## Docker Deployment

The application can be containerized using Docker:

```bash
# Build the Docker image
docker build -t team-maker .

# Run the container
docker run -p 8000:8000 team-maker
```

## Requirements

### For C++ Components:

- C++ compiler (gcc/g++ recommended)
- Standard C++ libraries

### For API Components:

- Python 3.6 or higher
- FastAPI
- Uvicorn
- Python-multipart

## License

This project is licensed under the MIT License. See the LICENSE file for details.
