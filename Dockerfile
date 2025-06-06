FROM python:3.9-slim

WORKDIR /app

# Install necessary tools to compile the C++ code
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Copy all project files
COPY . .

# Create bin directory and frontend directory if they don't exist
RUN mkdir -p bin
RUN mkdir -p frontend

# Copy frontend files directly to /app/frontend (not relative path)
COPY frontend/ /app/frontend/

# Compile C++ files
RUN g++ -o bin/team_maker_headers.exe cpp/src/team_maker_headers.cpp
RUN g++ -o bin/team_maker_api.exe cpp/src/team_maker_api.cpp cpp/src/Person.cpp cpp/src/Team.cpp cpp/src/TeamGenerator.cpp cpp/src/RandomTeamGenerator.cpp cpp/src/RandomCategoricalTeamGenerator.cpp cpp/src/Utilities.cpp

# Install Python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Set environment variables
ENV PORT=8000

# Use shell form of CMD so environment variables are properly expanded
CMD uvicorn api.main:app --host 0.0.0.0 --port ${PORT}