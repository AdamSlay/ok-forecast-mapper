import asyncio
import csv
from datetime import datetime
import logging
import os
import socket
from zoneinfo import ZoneInfo

import aiohttp
from meteostat import Stations
import requests

from src.api import Forecast

# Initialize logger so it can be recognized globally
mapper_log = logging.getLogger(f'data-mapper: {__name__}')


def main() -> int:
    os.makedirs("/log/logs", exist_ok=True)
    log_date = datetime.now(tz=ZoneInfo('US/Central')).strftime('%Y-%m-%d')
    log_time = datetime.now(tz=ZoneInfo('US/Central')).strftime('%Y-%m-%d %I:%M:%S')
    logging.basicConfig(format=f'[{log_time}]  [%(name)s]  [%(levelname).1s]  %(message)s',
                        encoding='utf-8',
                        filename=f'/log/logs/{log_date}.log',
                        level=logging.INFO)
    mapper_log.info("-----| Data-Mapper Started  |-----")

    try:
        oklahoma = ["Oklahoma", "OK", "40"]  # [State, Abbreviation, FP code]
        asyncio.run(get_stations(oklahoma))
    except Exception as e:
        mapper_log.fatal(f"There was an exception while executing loop() in mapper.py: {e}")
        raise Exception

    try:
        command_request('png-mapper', 8000, 'png_mapper_exe')
    except Exception as e:
        mapper_log.fatal(f"There was an exception while executing command_request() in mapper.py: {e}")
        raise Exception

    mapper_log.info("-----| Data-Mapper Finished |-----")
    return 0


async def get_stations(state: list) -> None:
    """
    Get station data and create async task
    :param state: List that includes ["State Name", "State Abbreviation", "State FP Code"]
    :return: None
    """
    state_abv = state[1]
    stations = create_df(state_abv)

    task = asyncio.create_task(fetch_data(stations, state_abv))
    await task


def create_df(state_abv: str) -> Stations:
    """
    Use the Stations class from meteostat to fetch station data for every available station within the given state
    If station data is currently unavailable, prune from DataFrame
    :param state_abv: 2-letter abbreviation of the given state
    :return: Pandas DataFrame containing station data from each station in the state
    """
    try:
        stations = Stations()
        stations = stations.region('US', state_abv)
        stations = stations.fetch()
        exclude = ['N/A']
        stations = stations[~stations.icao.isin(exclude)]  # exclude invalid stations
        return stations
    except Exception as e:
        mapper_log.warning(f"Error in create_df() while working on {state_abv}: {e}")


async def fetch_data(stations: Stations, state_abv: str) -> None:
    """
    Iterate through the stations within each state and fetch the forecast data from
    api.weather.gov. After fetching the data, create a .csv file and save that file to the
    shared volume
    :param stations: Pandas DataFrame of the station data for each station in the given state
    :param state_abv: 2-letter abbreviation of the state currently being iterated
    :return: None
    """
    tz_pref = ZoneInfo("US/Central")
    day = datetime.now(tz=tz_pref).strftime('%Y-%m-%d')
    hour = datetime.now(tz=tz_pref).strftime('%Y-%m-%dT%H')
    dir_path = f'/vol/data-vol/{str(day)}'
    os.makedirs(dir_path, exist_ok=True)

    # fetch data from weather.gov api and put in Queue
    stat_data = []
    conn = aiohttp.TCPConnector(family=socket.AF_INET, ssl=False, )
    coords = stations[["latitude", "longitude"]]
    async with aiohttp.ClientSession(connector=conn, trust_env=True) as session:

        for row, st_id in enumerate(stations["icao"]):
            loc = coords.iloc[row]
            api_req = Forecast(loc, session)
            forecast_url = await api_req.get_json()

            if forecast_url:
                await api_req.get_forecast(forecast_url, stat_data)
                # forecast_args order: temp, windSp, windDir, lat, lon
            else:
                stat_data.append([1000, 1000, 1000, loc["latitude"], loc["longitude"]])

    # Write data to csv and save to shared vol
    fn = f"{state_abv}-{hour}.csv"
    with open(rf"/vol/data-vol/{day}/{fn}", "w", newline="\n") as file:
        fields = ['temp(F)', 'windSp(mph)', 'windDir', 'lat', 'lon']
        write = csv.writer(file)
        write.writerow(fields)
        write.writerows(stat_data)
    mapper_log.info(f'Successfully saved forecast data to /vol/data-vol/{day}/{fn}')


def command_request(host: str, port: int, command: str) -> None:
    """
    Send JSON via POST request to CPP application
    :param command: The command to run in the CPP application
    :param host: Name of the container being connected to - in this case 'png-mapper'
    :param port: Port number to connect to on 'png-mapper'
    :return: None
    """
    mapper_log.info(f'Sending request to {host} on port {port}. Command: {command}')
    tz_pref = ZoneInfo("US/Central")
    day = datetime.now(tz=tz_pref).strftime('%Y-%m-%d')
    hour = datetime.now(tz=tz_pref).strftime('%Y-%m-%dT%H')
    path = f'{day}/'
    file = f'{hour}'

    resp = requests.post(f'http://{host}:{port}/{command}', json={"args": [path, file]})
    mapper_log.info(f'Response from {host}: {resp.json()}')


if __name__ == "__main__":
    main()
