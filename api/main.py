import os
from fastapi import FastAPI, UploadFile, File, Form, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse, JSONResponse
import subprocess
import tempfile
import shutil
import json
from typing import List, Optional
from pydantic import BaseModel

app = FastAPI(title="Team Maker API")

# Enable CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class CategoryWeight(BaseModel):
    index: int
    weight: float
    name: str

class TeamGenerationRequest(BaseModel):
    num_teams: int
    generation_type: str  # 'random', 'categorical', 'random_categorical'
    categories: Optional[List[CategoryWeight]] = None
    file_path: str

class SearchByCategoryRequest(BaseModel):
    file_path: str
    generation_type: str
    num_teams: int
    category_index: int
    categories: List[CategoryWeight]
    
@app.post("/upload-csv/")
async def upload_csv(file: UploadFile = File(...)):
    try:
        # Create a temporary file to save the uploaded CSV
        temp_dir = tempfile.mkdtemp()
        file_path = os.path.join(temp_dir, "uploaded_data.csv")
        
        # Save the uploaded file
        with open(file_path, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
        
        # Verify CSV is properly formatted - basic check
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                first_line = f.readline().strip()
                if not first_line or ',' not in first_line:
                    return JSONResponse(
                        status_code=400,
                        content={"error": "Invalid CSV format. File must contain comma-separated headers."}
                    )
        except UnicodeDecodeError:
            return JSONResponse(
                status_code=400,
                content={"error": "Invalid file encoding. Please use UTF-8."}
            )
        
        # Determine the base directory (handles both local and deployed environments)
        base_dir = os.getcwd()
        bin_dir = os.path.join(base_dir, "bin")
        
        # Handle case where we're in the API directory locally
        if os.path.basename(base_dir) == "api":
            bin_dir = os.path.join(os.path.dirname(base_dir), "bin")
        
        # Run the C++ executable to get the headers
        cpp_exec_path = os.path.join(bin_dir, "team_maker_headers.exe")
        
        # Log the path to help with debugging
        print(f"Using executable at: {cpp_exec_path}")
        print(f"File path: {file_path}")
        
        # Make sure the executable exists
        if not os.path.exists(cpp_exec_path):
            return JSONResponse(
                status_code=500,
                content={"error": "C++ executable not found. Make sure to build the headers utility."}
            )
        
        # Run the executable to extract headers with better error handling
        try:
            result = subprocess.run(
                [cpp_exec_path, file_path],
                capture_output=True,
                text=True,
                check=False  # Don't raise exception, handle manually
            )
            
            if result.returncode != 0:
                return JSONResponse(
                    status_code=400,
                    content={"error": f"Error processing CSV: {result.stderr}"}
                )
            
            # Clean the output to remove any control characters
            cleaned_output = ''.join(c for c in result.stdout if ord(c) >= 32 or c in ['\n', '\r', '\t'])
            
            # Try parsing the JSON output
            try:
                headers = json.loads(cleaned_output)
                
                # Additional validation
                if not headers or not isinstance(headers, list):
                    return JSONResponse(
                        status_code=400,
                        content={
                            "error": "Invalid headers format", 
                            "raw_output": cleaned_output[:200]
                        }
                    )
                
                # Filter out any empty headers or newline characters
                headers = [h for h in headers if h and h.strip() and not h.startswith('\n') and not h.startswith('\r')]
                
                return {
                    "message": "CSV uploaded successfully",
                    "headers": headers,
                    "file_path": file_path
                }
            except json.JSONDecodeError as e:
                return JSONResponse(
                    status_code=400,
                    content={
                        "error": f"Failed to parse headers: {str(e)}", 
                        "raw_output": cleaned_output[:200]
                    }
                )
        except Exception as e:
            return JSONResponse(
                status_code=500,
                content={"error": f"Error running headers executable: {str(e)}"}
            )
    except Exception as e:
        return JSONResponse(
            status_code=500,
            content={"error": f"Failed to process the file: {str(e)}"}
        )

@app.post("/generate-teams/")
async def generate_teams(request: TeamGenerationRequest):
    try:
        file_path = request.file_path
        # Check if file exists
        if not os.path.exists(file_path):
            raise HTTPException(status_code=400, detail="File not found. Please upload the CSV file again.")
        
        # Determine the base directory (handles both local and deployed environments)
        base_dir = os.getcwd()
        bin_dir = os.path.join(base_dir, "bin")
        
        # Handle case where we're in the API directory locally
        if os.path.basename(base_dir) == "api":
            bin_dir = os.path.join(os.path.dirname(base_dir), "bin")
        
        # Set up the command to run the team generator executable
        cpp_exec_path = os.path.join(bin_dir, "team_maker_api.exe")
        
        # Prepare command arguments
        command = [cpp_exec_path, file_path, "generate", request.generation_type, str(request.num_teams)]
        
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

@app.post("/search-teams-by-category/")
async def search_teams_by_category(request: SearchByCategoryRequest):
    try:
        file_path = request.file_path
        # Check if file exists
        if not os.path.exists(file_path):
            raise HTTPException(status_code=400, detail="File not found. Please upload the CSV file again.")
        
        # Determine the base directory
        base_dir = os.getcwd()
        bin_dir = os.path.join(base_dir, "bin")
        
        # Handle case where we're in the API directory locally
        if os.path.basename(base_dir) == "api":
            bin_dir = os.path.join(os.path.dirname(base_dir), "bin")
        
        # Set up the command
        cpp_exec_path = os.path.join(bin_dir, "team_maker_api.exe")
        
        # Prepare command arguments
        command = [
            cpp_exec_path, 
            file_path, 
            "search", 
            request.generation_type, 
            str(request.num_teams),
            str(request.category_index)
        ]
        
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
                content={"error": f"Error searching teams: {result.stderr}"}
            )
        
        # Parse the output
        try:
            search_results = json.loads(result.stdout)
            return search_results
        except json.JSONDecodeError:
            return JSONResponse(
                status_code=500,
                content={"error": "Failed to parse search results", "raw_output": result.stdout[:200]}
            )
    except Exception as e:
        return JSONResponse(
            status_code=500,
            content={"error": f"Failed to search teams: {str(e)}"}
        )

# Determine the frontend directory based on the environment
frontend_dir = os.path.join(os.getcwd(), "frontend")

# Check if the directory exists (for local development)
if not os.path.exists(frontend_dir):
    # Try to find it relative to the API directory
    parent_dir = os.path.dirname(os.getcwd())
    frontend_dir = os.path.join(parent_dir, "frontend")

# Only mount static files if directory exists
if os.path.exists(frontend_dir):
    app.mount("/", StaticFiles(directory=frontend_dir, html=True), name="frontend")

    @app.get("/")
    def read_root():
        return FileResponse(os.path.join(frontend_dir, "index.html"))
else:
    @app.get("/")
    def read_root():
        return {"message": "API is running. Frontend directory not found."}