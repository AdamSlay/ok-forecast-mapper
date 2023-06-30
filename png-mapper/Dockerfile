FROM ubuntu:jammy as builder
ARG DEBIAN_FRONTEND=noninteractive
ARG UID
ARG GID
RUN apt-get update && apt-get install -y --no-install-recommends \
      git \
      apt-transport-https \
      ca-certificates \
      build-essential \
      cmake \
      clang \
      gdb \
      python3 \
      && groupadd -g ${GID} png-group && useradd -u ${UID} -g png-group png-user \
      && mkdir -p /mapper/vol /base/images /spack \
      && git clone -c feature.manyFiles=true https://github.com/spack/spack.git

COPY png-mapper/etc/config.yaml /spack/etc/spack/defaults
RUN cd /spack/bin \
    && ./spack install pngwriter@0.7.0

COPY png-mapper/ /mapper
RUN cd /mapper \
    && chown -R png-user:png-group /mapper \
    && sh ./build.sh

FROM builder as test
ARG DEBIAN_FRONTEND=noninteractive

RUN chown -R png-user /mapper \
    && chmod +x /mapper/test.sh

WORKDIR /mapper
USER png-user

FROM ubuntu:jammy
ARG DEBIAN_FRONTEND=noninteractive
ARG UID
ARG GID

RUN groupadd -g ${GID} png-group && useradd -u ${UID} -g png-group png-user

COPY --from=builder /spack/opt/spack /spack/opt/spack
COPY --from=builder --chown=png-user /mapper /mapper

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    pip \
    && mkdir -p /base/images \
    && chmod +x /mapper/src/build/png_mapper_exe \
    && python3 -m pip install /mapper/httpexec/ hypercorn

WORKDIR /mapper
USER png-user
