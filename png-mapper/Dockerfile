FROM ubuntu:jammy as builder
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
      git \
      apt-transport-https \
      ca-certificates \
      build-essential \
      cmake \
      clang \
      gdb \
      python3 \
      && mkdir -p /mapper/vol /base/images /spack \
      && git clone -c feature.manyFiles=true https://github.com/spack/spack.git

COPY png-mapper/etc/config.yaml /spack/etc/spack/defaults
RUN cd /spack/bin \
    && ./spack install pngwriter@0.7.0

COPY png-mapper/ /mapper
RUN cd /mapper \
    && sh ./build.sh


FROM builder as test
ARG DEBIAN_FRONTEND=noninteractive
ARG UID
ARG GID

RUN groupadd -g ${GID} png-group && useradd -m -u ${UID} -g png-group png-user \
    && chown -R png-user
