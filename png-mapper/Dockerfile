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


FROM ubuntu:jammy
ARG DEBIAN_FRONTEND=noninteractive

COPY --from=builder /spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0 /spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0
COPY --from=builder --chown=png-user /mapper /mapper

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    pip \
    && useradd -u 8887 png-user \
    && mkdir -p /base/images \
    && chmod +x /mapper/src/build/png_mapper_exe \
    && python3 -m pip install /mapper/httpexec/ hypercorn

USER png-user

