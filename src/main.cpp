#include <fftw3.h>                      // For performing fft
#include <portaudio.h>                  // For audio sampling
#include "MQTTClient.h"                  // For sending data
#include <string>                       // Making a message to be sent
#include <ctime>
#include <exception>
#include <iostream>
#include "lib.h"
#include "Message.hpp"

#define MQTT_TOPIC "Home"         // Change this variable for each sensor
#define MQTT_HOSTNAME "localhost"
#define MQTT_PORT 1883
#define CLIENTID "Thibault"             // Change this variable for each sensor
#define TIMEOUT 2000L

class Portaudio
{
    public:
        Portaudio()
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
            inputParameters.hostApiSpecificStreamInfo = NULL;
        }
        
        ~Portaudio()
        {
            Pa_Terminate();
        }

        PaStreamParameters* get_parameters()
        {
            return &inputParameters;
        }

        void open_stream(unsigned long size, double sample_rate, PaStreamFlags flag)
        {
            PaError err;
            err = Pa_OpenStream(&stream, &inputParameters, nullptr, sample_rate, size, flag, nullptr, nullptr);
            if (err != paNoError)
            {
                throw std::runtime_error("Could not open Portaudio stream");
            }
        }

        void start_stream()
        {
            PaError err;
            err = Pa_StartStream(stream);

            if (err != paNoError)
            {
                throw std::runtime_error("Could not Start Portaudio stream");
            }
        }

        void read_stream(float *data, int size)
        {
            PaError err;
            err = Pa_ReadStream(stream, data, size);
            if (err != paNoError)
            {
                throw std::runtime_error("Could not read data from Portaudio stream");
            }
        }

    private:
        PaStreamParameters inputParameters;
        PaStream *stream;
};

int main()
{
    int fft_size = 8192;
    int sample_rate = 44100;
    // Applying a fft transform on data halves the amount of data available
    int fft_result = (fft_size) / 2;
    int qos = 0;


    // Initialize the mosquitto client that will be used
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    
    int rc;

    MQTTClient_create(&client,
                    MQTT_HOSTNAME,
                    CLIENTID,
                    MQTTCLIENT_PERSISTENCE_NONE,
                    NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    // data is going to be where audio data is stored
    // data will be the input to fft
    // out will be the output
    float data[fft_size];
    fftwf_complex out[fft_size / 2];

    float message[fft_size/2];

    // Initialize PortAudio
    Portaudio PA;
    PA.open_stream(fft_size, sample_rate, paClipOff);
    PA.start_stream();
    
    int pt = 0;
    float lat = 0; // Latency variable
    while (true)
    {
        int t = time(nullptr);
        if (t - pt > 4 && t % 5 == 0)
        {    
            pt = t;
            // Create the fftw plan
            fftwf_plan plan = fftwf_plan_dft_r2c_1d(fft_size, data, out, FFTW_ESTIMATE);        
            
            // Pa_ReadStream is a blocking call to take in mic input
            PA.read_stream(data, fft_size);
            fftwf_execute(plan);

            // Here, I prepare the message that will be sent over MQTT
            Message m(MQTT_TOPIC, out, fft_result, t, lat);
            
            pubmsg.payload = m.get_message();
            pubmsg.payloadlen = m.get_length();
            pubmsg.qos = qos;
            pubmsg.retained = 0;

            clock_t time_before_publish = clock();
            MQTTClient_publishMessage(client, MQTT_TOPIC, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            clock_t time_after_publish = clock();
            clock_t diff = time_after_publish - time_before_publish;
            lat = ((float)diff)/CLOCKS_PER_SEC;

            Pa_Sleep(3000);
        }

        else
        {
            Pa_Sleep(1000);
        }
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}