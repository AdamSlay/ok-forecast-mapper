import csv
import socket
from datetime import datetime
import logging
import os

import aiohttp
import requests
from pandas import DataFrame
import pytest
from zoneinfo import ZoneInfo

from src.mapper import get_stations, init_logging, make_shared_dirs, write_csv, command_request, fetch_data

# Create global day/hour vars
tz_pref = ZoneInfo("US/Central")
day = datetime.now(tz=tz_pref).strftime('%Y-%m-%d')
hour = datetime.now(tz=tz_pref).strftime('%Y-%m-%dT%H')


class MockedStations(DataFrame):
    def region(self, country, state):
        return self

    def fetch(self):
        return [{'icao': 'OK123'}, {'icao': 'N/A'}]


class MockedForecast:
    def __init__(self, loc: list, session: aiohttp.ClientSession):
        self.loc = loc
        self.session = session

    def get_json(self):
        return "test.forecast.url"

    def get_forecast(self, forecast_url: str, stat_data: list):
        stat_data.append([1, 2, 'mph', 4.0, 5.0])
        stat_data.append([6, 7, 'mph', 9.0, 10.0])
        return stat_data


def test_init_logging(mocker):
    m_makedirs = mocker.patch('os.makedirs')
    m_logging = mocker.patch('logging.basicConfig')

    init_logging()
    log_date = datetime.now(tz=ZoneInfo('US/Central')).strftime('%Y-%m-%d')
    log_time = datetime.now(tz=ZoneInfo('US/Central')).strftime('%Y-%m-%d %I:%M:%S')

    assert m_makedirs.called_once_with('/log/logs', exist_ok=True)
    assert m_logging.called_once_with(format=f'[{log_time}]  [%(name)s]  [%(levelname).1s]  %(message)s',
                                      encoding='utf-8',
                                      filename=f'/log/logs/{log_date}.log',
                                      level=logging.INFO)


def test_make_shared_dirs(mocker):
    m_makedirs = mocker.patch('os.makedirs')
    make_shared_dirs()
    assert m_makedirs.called_once_with(f'/vol/data-vol/{day}', exist_ok=True)


@pytest.mark.asyncio
async def test_get_stations(mocker):
    state = 'OK'
    m_Stations = mocker.patch('src.mapper.Stations', return_value=MockedStations())
    m_fetch_data = mocker.patch('src.mapper.fetch_data')

    await get_stations(state)

    m_Stations.assert_called_once_with()
    stations = m_Stations().fetch()
    m_fetch_data.assert_called_once_with(stations)


@pytest.mark.asyncio
async def test_fetch_data(mocker):
    stations = DataFrame(
        [{'icao': 'OK123', 'latitude': '30', 'longitude': '97'}, {'icao': 'N/A', 'latitude': '30', 'longitude': '98'}]
    )
    conn = aiohttp.TCPConnector(family=socket.AF_INET, ssl=False)
    test_stat_data = [[1, 2, 'mph', 4.0, 5.0], [6, 7, 'mph', 9.0, 10.0]]  # Mock data from MockedForecast.get_forecast()

    m_session = mocker.patch('aiohttp.ClientSession')
    m_Forecast = mocker.patch('src.api.Forecast', return_value=MockedForecast([30.0, 97.0], aiohttp.ClientSession()))
    m_write_csv = mocker.patch('src.mapper.write_csv')

    await fetch_data(stations)

    assert m_session.called_once_with(connector=conn, trust_env=True)
    assert m_Forecast.called_once_with([30.0, 97.0], m_session)
    assert m_Forecast.get_json.called_once()
    assert m_Forecast.get_forecast.called_once_with("test.forecast.url", [])
    assert m_write_csv.called_once_with(test_stat_data, f'/vol/data-vol/{day}/{hour}.csv')


def test_write_csv():
    stat_data = [
        [1, 2, 'mph', 4.0, 5.0],
        [6, 7, 'mph', 9.0, 10.0],
        [11, 12, 'mph', 14.0, 15.0]
    ]

    with open("test.csv", "w") as file:
        write_csv(stat_data, file.name)

    with open("test.csv", "r") as file:
        reader = csv.reader(file)
        data = list(reader)
        assert data[0] == ["temp(F)", "windSp(mph)", "windDir", "lat", "lon"]
        assert data[1] == [str(i) for i in stat_data[0]]
        assert data[3] == [str(i) for i in stat_data[2]]
    os.remove("test.csv")


def test_command_request(mocker):
    host = 'test-host'
    port = 8080
    command = 'run'
    path = f'{day}/'
    file = f'{hour}'

    m_post = mocker.patch.object(requests, 'post')
    command_request(host, port, command)
    m_post.assert_called_once_with(f'http://{host}:{port}/{command}', json={"args": [path, file]})
