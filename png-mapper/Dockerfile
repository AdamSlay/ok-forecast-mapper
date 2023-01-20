FROM ubuntu:jammy as base
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      git \
      apt-transport-https \
      ca-certificates \
      python3 \
      python3.10-venv \
      pip \
      build-essential \
      cmake \
      clang \
      gdb && \
      mkdir -p /mapper/vol /base/images &&  \
      useradd -u 8887 png-user &&  \
      chown png-user /mapper /mapper/vol /base/images /home

RUN git clone -c feature.manyFiles=true https://github.com/spack/spack.git
COPY png-mapper/etc/config.yaml spack/etc/spack/defaults
RUN cd spack/bin && ./spack install pngwriter@0.7.0


FROM base as builder

COPY --chown=png-user: png-mapper/ /mapper

RUN python3 -m venv .venv && . .venv/bin/activate
RUN python3 -m pip install /mapper/httpexec/ hypercorn
USER png-user
