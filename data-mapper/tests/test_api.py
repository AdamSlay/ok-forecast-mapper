from unittest.mock import AsyncMock

import pytest

from src.api import Forecast


def test_init(mocker):
    m_session = mocker.patch('aiohttp.ClientSession')
    t_forecast = Forecast([30.0, 97.0], m_session)
    assert t_forecast.session == m_session
    assert t_forecast.lat == 30.0
    assert t_forecast.lon == 97.0


@pytest.mark.asyncio
async def test_get_json(mocker):
    m_session = mocker.patch('aiohttp.ClientSession')
    m_response = mocker.patch('aiohttp.ClientResponse')
    m_session.get = AsyncMock(return_value=m_response)
    m_response.json = AsyncMock(return_value={'properties': {'forecastHourly': 'test.forecast.url'}})
    m_forecast = Forecast([30.0, 97.0], m_session)
    forecast_url = await m_forecast.get_json()
    assert forecast_url == "test.forecast.url"


@pytest.mark.asyncio
async def test_get_forecast(mocker):
    m_session = mocker.patch('aiohttp.ClientSession')
    m_response = mocker.patch('aiohttp.ClientResponse')
    m_session.get = AsyncMock(return_value=m_response)
    m_response.json = AsyncMock(return_value=
    {'properties':
        {'periods':
            [
                {'temperature': 1, 'windSpeed': '2 mph', 'windDirection': 'SE'},
                {'temperature': 6, 'windSpeed': '7 mph', 'windDirection': 'NE'}
            ]
        }
    }
    )
    m_forecast = Forecast([30.0, 97.0], m_session)
    forecast_data = await m_forecast.get_forecast("test.forecast.url", [])
    assert forecast_data == [[1, '2', 'SE', 30.0, 97.0]]


@pytest.mark.asyncio
async def test_get_forecast_invalid(mocker):
    m_session = mocker.patch('aiohttp.ClientSession')
    m_response = mocker.patch('aiohttp.ClientResponse')
    m_session.get = AsyncMock(return_value=m_response)
    m_response.json = AsyncMock(return_value=
    {'Unexpected JSON':
        {'periods':
            [
                {'temperature': 1, 'windSpeed': '2 mph', 'windDirection': 'SE'},
                {'temperature': 6, 'windSpeed': '7 mph', 'windDirection': 'NE'}
            ]
        }
    }
    )
    m_forecast = Forecast([30.0, 97.0], m_session)
    forecast_data = await m_forecast.get_forecast("test.forecast.url", [])
    assert forecast_data == [[1000, 1000, 1000, 30.0, 97.0]]
