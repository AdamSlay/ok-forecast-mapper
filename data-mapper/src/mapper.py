import asyncio
import csv
from datetime import datetime
import logging
import os
from pandas import Series
from zoneinfo import ZoneInfo

import aiohttp
from meteostat import Stations
import requests

from src.api import Forecast

MAPPER_PATH_PREFIX = '/home/mapper-user/mapper'

# Initialize logger so it can be recognized globally
mapper_log = logging.getLogger(f'data-mapper: {__name__}')

# Create global day/hour vars
tz_pref = ZoneInfo("US/Central")
day = datetime.now(tz=tz_pref).strftime('%Y-%m-%d')
hour = datetime.now(tz=tz_pref).strftime('%Y-%m-%dT%H')


def init_logging() -> None:
    """
    Initialize logger and create /logs on host if not already created
    """
    os.makedirs(f"{MAPPER_PATH_PREFIX}/log/logs", exist_ok=True)
    log_date = datetime.now(tz=ZoneInfo('US/Central')).strftime('%Y-%m-%d')
    log_time = datetime.now(tz=ZoneInfo('US/Central')).strftime('%Y-%m-%d %I:%M:%S')
    logging.basicConfig(format=f'[{log_time}]  [%(name)s]  [%(levelname).1s]  %(message)s',
                        encoding='utf-8',
                        filename=f'{MAPPER_PATH_PREFIX}/log/logs/{log_date}.log',
                        level=logging.INFO)


def make_shared_dirs() -> None:
    """
    Create shared data volume on docker network that links to png-mapper container
    """
    dir_path = f'{MAPPER_PATH_PREFIX}/vol/data-vol/{str(day)}'
    os.makedirs(dir_path, exist_ok=True)


async def get_stations(state: str) -> None:
    """
    Get station data and create async task
    :param state: state abbreviation - "OK"
    :return: None
    """
    stations = Stations()
    stations = stations.region('US', state)
    stations = stations.fetch()

    # task = asyncio.create_task(fetch_data(stations))
    # await task
    stat_data = []
    async with aiohttp.ClientSession() as session:
        tasks = []
        for i, station in stations.iterrows():
            tasks.append(fetch_data(station, session, stat_data))
        data = await asyncio.gather(*tasks)

    fn = f"{MAPPER_PATH_PREFIX}/vol/data-vol/{day}/{hour}.csv"
    write_csv(stat_data, fn)


async def fetch_data(station: Series, session: aiohttp.ClientSession, stat_data: list) -> list:
    """
    Fetch the forecast data from api.weather.gov and append to stat_data
    :param station: Pandas DataFrame Series containing station data for the given station
    :param session: aiohttp ClientSession
    :param stat_data: List of station data for all stations
    :return: stat_data: List of station data
    """
    loc = station[["latitude", "longitude"]]
    api_req = Forecast(loc, session)
    forecast_url = await api_req.get_json()
    if forecast_url:
        await api_req.get_forecast(forecast_url, stat_data)
        # forecast_args order: temp, windSp, windDir, lat, lon
    else:
        stat_data.append([1000, 1000, 1000, loc["latitude"], loc["longitude"]])
    return stat_data


def write_csv(stat_data: list, fn: str) -> None:
    """
    Write data to csv and save to shared vol
    :param stat_data: List of station data to be saved as .csv file
    :param fn: Filename to be saved as
    """
    with open(rf"{fn}", "w", newline="\n") as file:
        fields = ['temp(F)', 'windSp(mph)', 'windDir', 'lat', 'lon']
        write = csv.writer(file)
        write.writerow(fields)
        write.writerows(stat_data)
    mapper_log.info(f'Successfully saved forecast data to {MAPPER_PATH_PREFIX}/vol/data-vol/{day}/{fn}')


def command_request(host: str, port: int, command: str) -> None:
    """
    Send JSON via POST request to CPP application
    :param command: The command to run in the CPP application
    :param host: Name of the container being connected to - in this case 'png-mapper'
    :param port: Port number to connect to on 'png-mapper'
    :return: None
    """
    mapper_log.info(f'Sending request to {host} on port {port}. Command: {command}')
    path = f'{day}/'
    file = f'{hour}'

    resp = requests.post(f'http://{host}:{port}/{command}', json={"args": [path, file]})
    mapper_log.info(f'Response from {host}: {resp.json()}')


def main() -> int:
    try:
        init_logging()
        mapper_log.info("-----| Data-Mapper Started  |-----")
    except Exception as e:
        mapper_log.fatal(f'There was an exception while initializing logging in mapper.py: {e}')
        return 1

    try:
        make_shared_dirs()
        asyncio.run(get_stations("OK"))
    except Exception as e:
        mapper_log.fatal(f"There was an exception while executing get_stations() in mapper.py: {e}")
        return 2

    try:
        command_request('png-mapper', 8000, 'png_mapper_exe')
    except Exception as e:
        mapper_log.fatal(f"There was an exception while executing command_request() in mapper.py: {e}")
        return 3

    mapper_log.info("-----| Data-Mapper Finished |-----")
    return 0


if __name__ == "__main__":
    main()
