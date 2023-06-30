FROM python:3.10 AS builder
ARG UID
ARG GID

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libatlas-base-dev \
    libgdal-dev \
    gfortran \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd -g ${GID} mapper-group && useradd -m -u ${UID} -g mapper-group mapper-user

RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

COPY /data-mapper .

RUN pip install --no-cache-dir -r requirements.txt \
    && pip install .

FROM builder as test
ARG UID
ARG GID

RUN groupadd -g ${GID} mapper-group && useradd -m -u ${UID} -g mapper-group mapper-user \
    && mkdir -p /vol/data-vol /log

COPY --from=builder --chown=${UID}:${GID} /opt/venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --no-cache-dir pytest

COPY --chown=${UID}:${GID} /data-mapper /home/mapper-user

WORKDIR /home/mapper-user
USER mapper-user

FROM builder
ARG UID
ARG GID

RUN groupadd -g ${GID} mapper-group && useradd -m -u ${UID} -g mapper-group mapper-user \
    && mkdir -p /vol/data-vol /log \
    && chown -R mapper-user:mapper-group /vol \
    && chmod -R 777 /log

COPY --from=builder --chown=${UID}:${GID} /opt/venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

WORKDIR /home/mapper-user
USER mapper-user
