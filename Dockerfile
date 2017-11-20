FROM resin/rpi-raspbian:jessie
MAINTAINER thibault

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    libasound-dev \
    mosquitto \
    mosquitto-clients \
    libmosquitto-dev \
    portaudio19-dev

ADD fftw-3.3.7.tar.gz /
RUN cd fftw-3.3.7 \
    && ./configure --enable-float && make \
    && make install

RUN cd
COPY . /
WORKDIR /

EXPOSE 1883

RUN make
CMD ["./main"]