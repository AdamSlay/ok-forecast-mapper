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
ARG USER_ID

RUN useradd -m -u ${USER_ID} png-user \
    && chown -R png-user /mapper \
    && chmod +x /mapper/test.sh

WORKDIR /mapper
USER png-user

FROM ubuntu:jammy
ARG DEBIAN_FRONTEND=noninteractive
ARG USER_ID

RUN useradd -m -u ${USER_ID} png-user \
    && mkdir -p /base/images

COPY --from=builder /spack/opt/spack /spack/opt/spack
COPY --from=builder --chown=${USER_ID} /mapper /mapper

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    pip \
    && python3 -m pip install /mapper/httpexec/ hypercorn \
    && chmod +x /mapper/src/build/png_mapper_exe

WORKDIR /mapper
USER png-user

