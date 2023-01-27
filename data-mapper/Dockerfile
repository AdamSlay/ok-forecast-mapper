FROM python:3.10 AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libatlas-base-dev \
    libgdal-dev \
    gfortran \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

COPY /data-mapper .

RUN pip install --no-cache-dir -r requirements.txt \
    && pip install .

FROM python:3.10

RUN useradd --create-home -u 8887 mapper-user \
    && mkdir -p /vol/data-vol /log

COPY --from=builder --chown=mapper-user /opt/venv /opt/venv

ENV PATH="/opt/venv/bin:$PATH"

WORKDIR /home/mapper-user
USER mapper-user
