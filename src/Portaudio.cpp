#include "Portaudio.hpp"

Portaudio::Portaudio()
{
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError)
    {
        throw std::runtime_error("Problem starting Portaudio");
    }
                
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device) -> defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;
}

Portaudio::~Portaudio()
{
    Pa_Terminate();
}

void Portaudio::open_stream(unsigned long size, double sample_rate, PaStreamFlags flag)
{
    PaError err;
    err = Pa_OpenStream(&stream, &inputParameters, nullptr, sample_rate, size, flag, nullptr, nullptr);
    if (err != paNoError)
    {
        throw std::runtime_error("Could not open Portaudio stream");
    }
}

void Portaudio::start_stream()
{
    PaError err;
    err = Pa_StartStream(stream);

    if (err != paNoError)
    {
        throw std::runtime_error("Could not Start Portaudio stream");
    }
}

void Portaudio::read_stream(float *data, int size)
{
    PaError err;
    err = Pa_ReadStream(stream, data, size);
    if (err != paNoError)
    {
        throw std::runtime_error("Could not read data from Portaudio stream");
    }
}