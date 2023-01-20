FROM python:3.10

RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  libatlas-base-dev \
  libgdal-dev \
  gfortran && \
  useradd --create-home -u 8887 mapper-user &&  \
  mkdir -p /vol/data-vol && \
  chown mapper-user /vol/data-vol

COPY data-mapper/ .

RUN  pip install --no-cache-dir -r requirements.txt

WORKDIR /home/mapper-user
USER mapper-user
