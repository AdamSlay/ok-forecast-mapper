# Forecast Mapper
This is a backend application that is designed to run every hour, the results of which can be fetched at any time via a RESTful API. 
The *data-mapper* image gathers the latest hourly weather forecast information from the NOAA API for all of the available stations in Oklahoma, and *png-mapper* generates .png map images of the state with the data clearly visualized.

## Usage
Building and running the application is pretty simple thanks to docker-compose and the run script. Simply clone the repo, then in the project folder run the following command:
```bash
./run_ok_forecast_mapper.sh prod
```
This will build and run the *prod* version of the application. To build and run the *test* version of the application, run the following command:
```bash
./run_ok_forecast_mapper.sh test
```
The images and logs will be saved in the project folder on the host machine for now.
