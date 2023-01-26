import aiohttp

from colorama import Fore


class Forecast:
    """
    Class that fetches data from api.weather.gov
    """
    def __init__(self, loc: list, session: aiohttp.ClientSession):
        self.lat = loc[0]  # latitude
        self.lon = loc[1]  # longitude
        self.session = session  # aiohttp.ClientSession

    async def get_json(self) -> str:
        """
        first api call, gets json which includes forecast url
        two-step process as per api docs: <https://weather-gov.github.io/api/general-faqs>
        :return: Forecast URL string for current lat,lon. This is the url for the most recent hourly forecast.
        """
        url = f"https://api.weather.gov/points/{self.lat},{self.lon}"
        print(Fore.WHITE + f'getting json - {url}', flush=True)
        try:
            get_req = await self.session.get(url, timeout=1)
            forecast_url = await get_req.json(content_type=None)

            if 'properties' in forecast_url:
                return forecast_url['properties']['forecastHourly']  # return the forecastHourly link
            else:
                print(Fore.CYAN + f"get_json returned None: {forecast_url}")
                return ""
            # await asyncio.sleep(10)

        except Exception as e:
            print(Fore.RED + f"Error in get_json() while requesting {url}: {e}", flush=True)
            return ""

    async def get_forecast(self, forecast_url: str, stat_data: list) -> list:
        """
        Second api call, gets hourly forecast from forecast URL
        :param forecast_url: Most recent hourly forecast URL returned by get_json()
        :param stat_data: List of values derived from hourly forecast [temp, wspd, wdir]
        :return: Stat_data with values inserted
        """
        print(Fore.WHITE + f'getting forecast for {self.lat}, {self.lon} - {forecast_url}', flush=True)
        try:
            get_req = await self.session.get(forecast_url)
            forecast_json = await get_req.json(content_type=None)

            if 'properties' in forecast_json:
                args = ["temperature", "windSpeed", "windDirection"]
                current_forecast = forecast_json['properties']['periods'][0]  # '0' refers to the most current forecast
                parms = [current_forecast[arg] for arg in args]
                parms[1] = parms[1].split(' ')[0]  # remove 'mph' from windspeed
                parms.append(self.lat)
                parms.append(self.lon)
                stat_data.append(parms)
                return stat_data

            elif 'properties' not in forecast_json:
                print(Fore.MAGENTA + f"Incorrect JSON while requesting {forecast_url}: {forecast_json}", flush=True)
                stat_data.append([1000, 1000, 1000, self.lat, self.lon])
                return stat_data

        except Exception as e:
            print(Fore.RED + f"Error in get_forecast() while requesting {self.lat}, {self.lon}: {e}", flush=True)
            stat_data.append([1000, 1000, 1000, self.lat, self.lon])
