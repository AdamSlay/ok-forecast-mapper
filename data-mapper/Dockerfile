FROM python:3.10

RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  libatlas-base-dev \
  libgdal-dev \
  gfortran

COPY data-mapper/requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt && useradd --create-home -u 8887 mapper-user && mkdir -p /src/data-vol

WORKDIR /home/mapper-user

COPY --chown=mapper-user: data-mapper/data data/
COPY --chown=mapper-user: data-mapper/src/ .

RUN chown mapper-user /src/data-vol

USER mapper-user
