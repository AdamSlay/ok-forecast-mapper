from setuptools import setup, find_packages

setup(name="mapper",
      version="0.1.0",
      description="Fetch station data from NOAA API",
      author="Adam Slay",
      author_email="aslay@ou.edu",
      packages=find_packages("."),
      install_requires=["aiohttp", "asyncio", "pandas", "meteostat", "requests"],
      entry_points={
          "console_scripts": [
              "mapper.py = src.mapper:main"
          ]
      }
      )
