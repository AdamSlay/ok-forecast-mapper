FROM python:3.10 AS builder

ARG USER_ID=502
ENV COMPOSE data-mapper/
ENV STANDALONE .
ENV COMPOSE_OR_STANDALONE_HOST_PATH ${COMPOSE}
ENV MAPPER_PATH_PREFIX /home/mapper-user/mapper

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libatlas-base-dev \
    libgdal-dev \
    gfortran \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && useradd -m -u ${USER_ID} mapper-user \
    && mkdir -p ${MAPPER_PATH_PREFIX}

RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

COPY ${COMPOSE_OR_STANDALONE_HOST_PATH} ${MAPPER_PATH_PREFIX}

RUN cd ${MAPPER_PATH_PREFIX} \
    && pip install --no-cache-dir -r requirements.txt \
    && pip install .

FROM builder as test
ARG USER_ID=502
ENV MAPPER_PATH_PREFIX /home/mapper-user/mapper

RUN mkdir -p ${MAPPER_PATH_PREFIX}/vol/data-vol ${MAPPER_PATH_PREFIX}/log

COPY --from=builder --chown=${USER_ID} /opt/venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --no-cache-dir pytest \
    && chown -R mapper-user ${MAPPER_PATH_PREFIX}

WORKDIR ${MAPPER_PATH_PREFIX}
USER mapper-user

FROM builder
ARG USER_ID=502
ENV MAPPER_PATH_PREFIX /home/mapper-user/mapper

RUN mkdir -p ${MAPPER_PATH_PREFIX}/vol/data-vol ${MAPPER_PATH_PREFIX}/log \
    && chown -R mapper-user ${MAPPER_PATH_PREFIX}/vol \
    && chmod -R 777 ${MAPPER_PATH_PREFIX}/log

COPY --from=builder --chown=${USER_ID} /opt/venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

WORKDIR ${MAPPER_PATH_PREFIX}
USER mapper-user
