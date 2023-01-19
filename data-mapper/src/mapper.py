import asyncio
import csv
from datetime import datetime
import os
import socket

import aiohttp
from colorama import Fore
from meteostat import Stations
from pandas import DataFrame
import requests

from api import Forecast

oklahoma = [["Oklahoma", "OK", "40"]]
# State, Abbreviation, FP code
us_states = [
    ["Alabama", "AL", "01"], ["Arizona", "AZ", "04"], ["Arkansas", "AR", "05"], ["California", "CA", "06"],
    ["Colorado", "CO", "08"], ["Connecticut", "CT", "09"], ["Delaware", "DE", "10"], ["Florida", "FL", "12"],
    ["Georgia", "GA", "13"], ["Idaho", "ID", "16"], ["Illinois", "IL", "17"], ["Indiana", "IN", "18"],
    ["Iowa", "IA", "19"], ["Kansas", "KS", "20"], ["Kentucky", "KY", "21"], ["Louisiana", "LA", "22"],
    ["Maine", "ME", "23"], ["Maryland", "MD", "24"], ["Massachusetts", "MA", "25"], ["Michigan", "MI", "26"],
    ["Minnesota", "MN", "27"], ["Mississippi", "MS", "28"], ["Missouri", "MO", "29"], ["Montana", "MT", "30"],
    ["Nebraska", "NE", "31"], ["Nevada", "NV", "32"], ["New Hampshire", "NH", "33"], ["New Jersey", "NJ", "34"],
    ["New Mexico", "NM", "35"], ["New York", "NY", "36"], ["North Carolina", "NC", "37"], ["North Dakota", "ND", "38"],
    ["Ohio", "OH", "39"], ["Oklahoma", "OK", "40"], ["Oregon", "OR", "41"], ["Pennsylvania", "PA", "42"],
    ["Rhode Island", "RI", "44"], ["South Carolina", "SC", "45"], ["South Dakota", "SD", "46"],
    ["Tennessee", "TN", "47"], ["Texas", "TX", "48"], ["Utah", "UT", "49"], ["Vermont", "VT", "50"],
    ["Virginia", "VA", "51"], ["Washington", "WA", "53"], ["West Virginia", "WV", "54"], ["Wisconsin", "WI", "55"],
    ["Wyoming", "WY", "56"],
]


async def main() -> int:
    await asyncio.gather(*map(loop, oklahoma))
    return 0


async def loop(state: list) -> None:
    """
    Async loop that controls program runtime
    :param state: List that includes ["State Name", "State Abbreviation", "State FP Code"]
    :return: None
    """
    # main loop for each state
    state_abv = state[1]
    stations = create_df(state_abv)

    task = asyncio.create_task(fetch_data(stations, state_abv))
    await task


def create_df(state_abv: str) -> DataFrame:
    """
    Use the Stations class from meteostat to fetch station data for every available station within the given state
    If station data is currently unavailable, prune from DataFrame
    :param state_abv: 2-letter abbreviation of the given state
    :return: Pandas DataFrame containing station data from each station in the state
    """
    # create dataframe with all stations in state using meteostat
    try:
        stations = Stations()
        stations = stations.region('US', state_abv)
        stations = stations.fetch()
        exclude = ['N/A']
        stations = stations[~stations.icao.isin(exclude)]  # exclude invalid stations
        return stations
    except Exception as e:
        print(Fore.RED + f"Error in create_df while working on {state_abv}: {e}")


async def fetch_data(stations: DataFrame, state_abv: str) -> None:
    """
    Iterate through the stations within each state and fetch the forecast data from
    api.weather.gov. After fetching the data, create a .csv file and save that file to the
    shared volume
    :param stations: Pandas DataFrame of the station data for each station in the given state
    :param state_abv: 2-letter abbreviation of the state currently being iterated
    :return: None
    """

    day = datetime.today().strftime('%Y-%m-%d')
    hour = datetime.utcnow().isoformat(timespec='hours')
    dir_path = f'/vol/data-vol/{str(day)}/{str(hour)}'
    try:
        os.makedirs(dir_path)
    except Exception as e:
        print(f"Could not create {dir_path}: {e}")

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
    with open(rf"/vol/data-vol/{day}/{hour}/{fn}", "w", newline="\n") as file:
        fields = ['temp(F)', 'windSp(mph)', 'windDir', 'lat', 'lon']
        write = csv.writer(file)
        write.writerow(fields)
        write.writerows(stat_data)


def command_request(host: str, port: int, command: str) -> None:
    """
    Send JSON via POST request to CPP application
    :param command: The command to run in the CPP application
    :param host: Name of the container being connected to - in this case 'png-mapper'
    :param port: Port number to connect to on 'png-mapper'
    :return: None
    """
    day = datetime.today().strftime('%Y-%m-%d')
    hour = datetime.utcnow().isoformat(timespec='hours')
    path = f'/mapper/vol/{day}/{hour}/OK-{hour}.csv'

    resp = requests.post(f'http://{host}:{port}/{command}', json={"args": [path]})
    print(resp.json())


if __name__ == "__main__":
    try:
        asyncio.run(main())
        command_request('png-mapper', 8000, 'png_mapper_exe')
    except Exception as ex:
        print(f"There was an exception in mapper.py: {ex}")
