#!/bin/bash
#tree /spack
cd src/ || exit
mkdir build images
cd build/ || exit
cmake ../../
make
chmod 755 png_mapper_exe

export HTTPEXEC_EXEC_ROOT
HTTPEXEC_EXEC_ROOT="/mapper/src/build/"

python3 -m hypercorn --error-logfile - --access-logfile - --bind png-mapper:8000 httpexec.asgi:app
