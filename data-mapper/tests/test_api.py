from src.api import Forecast
import pytest
from unittest.mock import AsyncMock


def test_init(mocker):
    m_session = mocker.patch('aiohttp.ClientSession')
    t_forecast = Forecast([30.0, 97.0], m_session)
    assert t_forecast.session == m_session
    assert t_forecast.lat == 30.0
    assert t_forecast.lon == 97.0


@pytest.mark.asyncio
async def test_get_json():
    pass


def test_get_forecast():
    pass
