import asyncio
import csv
from datetime import datetime
import logging
import os
import tempfile


import requests
from pandas import DataFrame
import pytest
from zoneinfo import ZoneInfo

from src.mapper import get_stations, init_logging, make_shared_dirs, write_csv, command_request

# Create global day/hour vars
tz_pref = ZoneInfo("US/Central")
day = datetime.now(tz=tz_pref).strftime('%Y-%m-%d')
hour = datetime.now(tz=tz_pref).strftime('%Y-%m-%dT%H')


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
    # mock the `Stations` class
    class MockedStations(DataFrame):
        def region(self, country, state):
            return self

        def fetch(self):
            return [{'icao': 'OK123'}, {'icao': 'N/A'}]

    # mock the `Stations` class to return a MockedStations object
    mStations = mocker.patch('src.mapper.Stations', return_value=MockedStations())

    # mock the `fetch_data` function
    fetch_data = mocker.patch('src.mapper.fetch_data')

    # set the state for the test
    state = 'OK'

    # call the function to be tested
    await get_stations(state)

    # assert that the `Stations` class was called with the expected arguments
    mStations.assert_called_once_with()

    # assert that the `fetch_data` function was called with the expected arguments
    stations = mStations().fetch()
    fetch_data.assert_called_once_with(stations)


def test_fetch_data():
    pass


# def test_write_csv():
#     # a list of two lists which have the following format: [int, int, str, double, double]
#     data = [[1, 2, 'test', 3.0, 4.0], [5, 6, 'test2', 7.0, 8.0]]
#     write_csv(data)
#     with open('/vol/data-vol/test_dir/test.csv', 'r') as f:
#         reader = csv.reader(f)
#         for row in reader:
#             assert row == data[0]
#             assert row == data[1]
#


# def test_write_csv(mocker):
#     stat_data = [
#         [1, 2, 'mph', 4.0, 5.0],
#         [6, 7, 'mph', 9.0, 10.0],
#         [11, 12, 'mph', 14.0, 15.0]
#     ]
#
#     with tempfile.NamedTemporaryFile(mode="w", delete=True) as file:
#         fn = file.name
#
#         with mocker.patch('builtins.open', mocker.mock_open):
#             write_csv(stat_data)
#
#         # Check that the file was created
#         assert os.path.exists(fn)
#
#         # Check that the correct data was written to the file
#         with open(fn, "r") as f:
#             reader = csv.reader(f)
#             data = list(reader)
#
#             assert data[0] == ["temp(F)", "windSp(mph)", "windDir", "lat", "lon"]
#             assert data[1:] == stat_data


def test_command_request(mocker):
    # set expected values for the test
    host = 'test-host'
    port = 8080
    command = 'run'
    path = f'{day}/'
    file = f'{hour}'

    # mock the post request
    mock_post = mocker.patch.object(requests, 'post')

    # call the function to be tested
    command_request(host, port, command)

    # assert that the post request was called with the expected arguments
    mock_post.assert_called_once_with(f'http://{host}:{port}/{command}', json={"args": [path, file]})
