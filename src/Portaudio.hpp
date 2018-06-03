#ifndef PORTAUDIO_HPP
#define PORTAUDIO_HPP

#include <portaudio.h>
#include <exception>
#include <iostream>

class Portaudio
{
    public:
        Portaudio();
        ~Portaudio();

        void open_stream(unsigned long size, double sample_rate, PaStreamFlags flag);
        void start_stream();
        void read_stream(float *data, int size);

    private:
        PaStreamParameters inputParameters;
        PaStream           *stream;
};

#endif