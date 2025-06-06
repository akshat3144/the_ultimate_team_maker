from fastapi import FastAPI, UploadFile, File, Form, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse, JSONResponse
import subprocess
import tempfile
import os
import json
import shutil
from typing import List, Optional
from pydantic import BaseModel

app = FastAPI(title="Team Maker API")

# Enable CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allows all origins
    allow_credentials=True,
    allow_methods=["*"],  # Allows all methods
    allow_headers=["*"],  # Allows all headers
)

class CategoryWeight(BaseModel):
    index: int
    weight: float
    name: str

class TeamGenerationRequest(BaseModel):
    num_teams: int
    generation_type: str  # 'random', 'categorical', 'random_categorical'
    categories: Optional[List[CategoryWeight]] = None
    file_path: str  # Add this line

@app.post("/upload-csv/")
async def upload_csv(file: UploadFile = File(...)):
    try:
        # Create a temporary file to save the uploaded CSV
        temp_dir = tempfile.mkdtemp()
        file_path = os.path.join(temp_dir, "uploaded_data.csv")
        
        # Save the uploaded file
        with open(file_path, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
        
        # Run the C++ executable to get the headers
        cpp_exec_path = os.path.join(os.getcwd(), "..", "bin", "team_maker_headers.exe")
        
        # Make sure the executable exists
        if not os.path.exists(cpp_exec_path):
            raise HTTPException(status_code=500, detail="C++ executable not found. Make sure to build the headers utility.")
        
        # Run the executable to extract headers
        result = subprocess.run(
            [cpp_exec_path, file_path],
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            return JSONResponse(
                status_code=400,
                content={"error": f"Error processing CSV: {result.stderr}"}
            )
        
        # Parse the output (headers)
        headers = json.loads(result.stdout)
        
        return {
            "message": "CSV uploaded successfully",
            "headers": headers,
            "file_path": file_path
        }
    except Exception as e:
        return JSONResponse(
            status_code=500,
            content={"error": f"Failed to process the file: {str(e)}"}
        )

@app.post("/generate-teams/")
async def generate_teams(request: TeamGenerationRequest):
    try:
        file_path = request.file_path  # Add file_path to the TeamGenerationRequest model
        # Check if file exists
        if not os.path.exists(file_path):
            raise HTTPException(status_code=400, detail="File not found. Please upload the CSV file again.")
        
        # Set up the command to run the team generator executable
        cpp_exec_path = os.path.join(os.getcwd(), "..", "bin", "team_maker_api.exe")
        
        # Prepare command arguments based on generation type
        command = [cpp_exec_path, file_path, request.generation_type, str(request.num_teams)]
        
        if request.generation_type in ["categorical", "random_categorical"]:
            if not request.categories or len(request.categories) == 0:
                raise HTTPException(status_code=400, detail="Categories required for categorical team generation")
            
            # Add category indices and weights
            category_indices = [str(cat.index) for cat in request.categories]
            weights = [str(cat.weight) for cat in request.categories]
            
            # Add to command
            command.append(",".join(category_indices))
            command.append(",".join(weights))
        
        # Run the executable
        result = subprocess.run(
            command,
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            return JSONResponse(
                status_code=400,
                content={"error": f"Error generating teams: {result.stderr}"}
            )
        
        # Parse the output (teams)
        teams = json.loads(result.stdout)
        
        return {
            "teams": teams
        }
    except Exception as e:
        return JSONResponse(
            status_code=500,
            content={"error": f"Failed to generate teams: {str(e)}"}
        )

# Mount the static files
app.mount("/", StaticFiles(directory="./frontend", html=True), name="frontend")

# Route for the main page
@app.get("/")
def read_root():
    return FileResponse("../frontend/index.html")