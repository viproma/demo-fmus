FROM ubuntu:latest

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata
RUN apt-get -y install git gcc g++ cmake
RUN apt-get -y install libboost-all-dev
RUN apt-get -y install vim

RUN mkdir -p /app
WORKDIR /app

COPY . /app

RUN cmake .
RUN make

