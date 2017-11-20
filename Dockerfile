FROM resin/rpi-raspbian:jessie
MAINTAINER thibault

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    libasound-dev \
    mosquitto \
    mosquitto-clients \
    mosquitto-dev \
    portaudio19-dev

ADD fftw-3.37.tar.gz
RUN cd fftw-3.37 \
    && ./configure --enable-float && make \
    && make install

RUN cd
COPY . /
WORKDIR /

EXPOSE 1883

RUN make
CMD ["./main"]
