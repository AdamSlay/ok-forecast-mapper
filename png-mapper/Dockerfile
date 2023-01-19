FROM ubuntu:jammy as base
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      git \
      apt-transport-https \
      ca-certificates \
      python3 \
      build-essential \
      cmake \
      clang \
      gdb
RUN git clone -c feature.manyFiles=true https://github.com/spack/spack.git
RUN rm spack/etc/spack/defaults/config.yaml
COPY png-mapper/etc/config.yaml spack/etc/spack/defaults
RUN cd spack/bin && ./spack install pngwriter@0.7.0


FROM base as builder
RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential \
      clang \
      gdb \
      cmake \
      ncat \
      python3 \
      python3.10-venv \
      pip \
      apt-transport-https \
      ca-certificates
RUN mkdir -p /mapper/vol /base/images /spack && useradd -u 8887 png-user && chown png-user /mapper /mapper/vol /base/images /spack /home
COPY --chown=png-user: png-mapper/ /mapper
COPY --from=base --chown=png-user: spack/ /spack

RUN python3 -m venv .venv
RUN . .venv/bin/activate
RUN python3 -m pip install /mapper/httpexec/ hypercorn
USER png-user
