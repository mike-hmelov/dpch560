FROM ubuntu:22.04

RUN apt update
RUN apt install -y pkg-config make g++ libhidapi-dev libsensors-dev

RUN useradd misha

USER misha

WORKDIR /app

ENTRYPOINT /bin/bash