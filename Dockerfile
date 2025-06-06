FROM python:3.9-slim

# Install build tools and C++ compiler
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy C++ source code
COPY cpp/ /app/cpp/

# Copy API code
COPY api/ /app/api/

# Copy frontend files
COPY frontend/ /app/frontend/

# Create bin directory and compile C++ executables
RUN mkdir -p /app/bin
RUN g++ -o /app/bin/team_maker_headers.exe /app/cpp/src/team_maker_headers.cpp
RUN g++ -o /app/bin/team_maker_api.exe \
    /app/cpp/src/team_maker_api.cpp \
    /app/cpp/src/Person.cpp \
    /app/cpp/src/Team.cpp \
    /app/cpp/src/TeamGenerator.cpp \
    /app/cpp/src/RandomTeamGenerator.cpp \
    /app/cpp/src/RandomCategoricalTeamGenerator.cpp \
    /app/cpp/src/Utilities.cpp \
    -I/app/cpp/include

# Install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Set environment variables
ENV PYTHONPATH=/app
ENV PORT=8000

# Set the working directory to the API folder
WORKDIR /app/api

# Expose the port
EXPOSE $PORT

# Start the FastAPI application
CMD uvicorn main:app --host 0.0.0.0 --port ${PORT}