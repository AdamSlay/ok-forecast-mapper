# Forecast Mapper
This is a backend application that is designed to run every hour, the results of which can be fetched at any time via a RESTful API. 
It gathers the latest hourly weather forecast information from the NOAA API for all of the available stations in the given state(at this time, only Oklahoma is supported).
The images are then saved to the server and available for retrieval via the Web Application.

## Usage
Running the application is pretty simple thanks to Docker Compose. Simply clone the repo, then in the project folder, run the following command:
```bash
docker compose up
```
The images will be saved in the project folder for now, I'm working on a database solution for those.
