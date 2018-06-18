FROM resin/rpi-raspbian:jessie AS builder
LABEL maintainer thibault.huyet@gmail.com

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    git \
    libasound-dev \
    libmosquitto-dev \
    libssl-dev \    
    mosquitto \
    mosquitto-clients \
    portaudio19-dev \
    && rm -rf /var/lib/apt/lists/*

ADD fftw-3.3.7.tar.gz /
RUN cd fftw-3.3.7 \
    && ./configure --enable-float && make \
    && make install

RUN cd ..

RUN git clone https://github.com/rpoisel/paho.mqtt.c.git
RUN cd paho.mqtt.c \
    make && make install

WORKDIR /
COPY . /
RUN make
EXPOSE 1883

CMD ["./main"]
