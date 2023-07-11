FROM ubuntu:jammy as builder
ARG DEBIAN_FRONTEND=noninteractive
ARG USER_ID=502
ENV MAPPER_PATH_PREFIX /home/png-user/mapper
ENV COMPOSE png-mapper
ENV STANDALONE .
ENV COMPOSE_OR_STANDALONE_HOST_PATH ${COMPOSE}

RUN apt-get update && apt-get install -y --no-install-recommends \
      git \
      apt-transport-https \
      ca-certificates \
      build-essential \
      cmake \
      clang \
      gdb \
      python3 \
      && useradd -m -u ${USER_ID} png-user \
      && mkdir -p ${MAPPER_PATH_PREFIX}/vol ${MAPPER_PATH_PREFIX}/base/images ${MAPPER_PATH_PREFIX}/spack \
      && cd ${MAPPER_PATH_PREFIX} \
      && git clone -c feature.manyFiles=true https://github.com/spack/spack.git

COPY ${COMPOSE_OR_STANDALONE_HOST_PATH}/etc/config.yaml ${MAPPER_PATH_PREFIX}/spack/etc/spack/defaults
RUN cd ${MAPPER_PATH_PREFIX}/spack/bin \
    && ./spack install pngwriter@0.7.0

COPY ${COMPOSE_OR_STANDALONE_HOST_PATH} ${MAPPER_PATH_PREFIX}
RUN cd ${MAPPER_PATH_PREFIX} \
    && sh ./build.sh

FROM builder as test
ARG DEBIAN_FRONTEND=noninteractive
ARG USER_ID=502
ENV MAPPER_PATH_PREFIX /home/png-user/mapper

RUN chown -R png-user ${MAPPER_PATH_PREFIX} \
    && chmod +x ${MAPPER_PATH_PREFIX}/test.sh

WORKDIR ${MAPPER_PATH_PREFIX}
USER png-user

FROM ubuntu:jammy
ARG DEBIAN_FRONTEND=noninteractive
ARG USER_ID=502
ENV MAPPER_PATH_PREFIX /home/png-user/mapper

RUN useradd -m -u ${USER_ID} png-user \
    && mkdir -p ${MAPPER_PATH_PREFIX}/base/images

COPY --from=builder --chown=${USER_ID} ${MAPPER_PATH_PREFIX} ${MAPPER_PATH_PREFIX}

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    pip


RUN python3 -m pip install ${MAPPER_PATH_PREFIX}/httpexec/ hypercorn \
    && chmod +x ${MAPPER_PATH_PREFIX}/src/build/png_mapper_exe \
    && chown -R png-user /home/png-user

USER png-user
WORKDIR ${MAPPER_PATH_PREFIX}
