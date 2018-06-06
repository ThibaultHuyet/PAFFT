FROM resin/rpi-raspbian:jessie
LABEL maintainer thibault.huyet@gmail.com

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    libasound-dev \
    mosquitto \
    mosquitto-clients \
    libarmadillo-dev \
    libblas-dev \
    liblapack-dev \
    libmosquitto-dev \
    portaudio19-dev \
    git \
    libssl-dev

ADD sigpack-1.2.4.zip /

ADD fftw-3.3.7.tar.gz /
RUN cd fftw-3.3.7 \
    && ./configure --enable-float && make \
    && make install

RUN cd ..

RUN git clone https://github.com/rpoisel/paho.mqtt.c.git
RUN cd paho.mqtt.c \
    make && make install

RUN cd ..
COPY . /
WORKDIR /

EXPOSE 1883

RUN make
CMD ["./main"]