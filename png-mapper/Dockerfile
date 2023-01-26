FROM ubuntu:jammy as base
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


FROM ubuntu:jammy
ARG DEBIAN_FRONTEND=noninteractive

COPY --from=base /mapper /mapper
COPY --from=base /spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0 /spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    pip \
    && python3 -m pip install /mapper/httpexec/ hypercorn \
    && mkdir -p /base/images \
    && useradd -u 8887 png-user \
    && chown png-user /base/images /mapper /spack \
    && chmod +x /mapper/src/build/png_mapper_exe

USER png-user
