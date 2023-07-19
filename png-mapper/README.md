# OFM-PNG-MAPPER

### This is the rendering application that renders and saves the `.png` map images for `ok-forecast-mapper`

## Usage
If you are wanting to run this container by itself without using the docker-compose file which builds and runs the entire application, you can do so by running the following commands to build and run the image(make sure to change COMPOSE_OR_STANDALONE_HOST_PATH to STANDALONE inside the Dockerfile):
```bash
docker build -t ofm-png-mapper ./<path-to-Dockerfile>
```
```bash
docker run -ti ofm-png-mapper sh
```
and once inside the container, run the following commands to set environment variables and start the application:
```bash
export HTTPEXEC_EXEC_ROOT='/home/png-user/mapper/src/build/'
```
```bash
python3 -m hypercorn --error-logfile - --access-logfile - --bind <host>:<port> httpexec.asgi:app
```
where `<host>` is the host machine and `<port>` is the port you want *httpexec* to listen to on the host.