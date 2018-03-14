#include <fftw3.h>                      // For performing fft
#include <portaudio.h>                  // For audio sampling
#include "MQTTClient.h"                  // For sending data
#include <string>                       // Making a message to be sent
#include <ctime>
#include <iostream>
#include "lib.h"
#include "Message.hpp"

#define MQTT_TOPIC "sound"
#define MQTT_HOSTNAME "localhost"
#define MQTT_PORT 1883
#define CLIENTID "Thibault"
#define TIMEOUT 2000L

int main()
{
    int fft_size = 8392;
    int sample_rate = 44100;
    // Applying a fft transform on data halves the amount of data available
    int fft_result = (fft_size - 200) / 2;
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

    PaStreamParameters inputParameters;                 // Input parameters for working with PortAudio
    PaError err = 0;                                    // err is for error checking
    PaStream *stream;                                   // Stream object created by PortAudio

    // data is going to be where audio data is stored
    // data will be the input to fft
    // out will be the output
    float data[fft_size];
    float reduced_data[fft_size - 200];
    fftwf_complex out[fft_size / 2];

    float message[fft_size/2];

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cout << "Problem initializing PortAudio\n";
        exit(-1);
    }

    // Define the kind of input devcie that we will be using for taking in audio
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device) -> defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    // Start the audio stream.
    err = Pa_OpenStream(&stream,                // Stream used for input
                        &inputParameters,       // Structure used to describe input parameters used by stream
                        NULL,                   // outputparameters: Since this is input only, this is NULL
                        sample_rate,            // sample rate
                        fft_size,               // frames per buffer
                        paClipOff,              // Not bothering to clip samples
                        NULL,                   // Using blocking API for now, so no callback or callback data
                        NULL                    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                        );

    if (err != paNoError)
    {
        std::cout << "Problem opening PortAudio stream\n";
        exit(-1);
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cout << "Problem starting PortAudio stream\n";
        exit(-1);
    }
    
    int pt = 0;

    while (true)
    {
        int t = time(nullptr);
        if (t - pt > 4 && t % 5 == 0)
        {    
            pt = t;
            // Create the fftw plan
            fftwf_plan plan = fftwf_plan_dft_r2c_1d(fft_size, reduced_data, out, FFTW_ESTIMATE);        
            
            // Pa_ReadStream is a blocking call to take in mic input
            err = Pa_ReadStream(stream, data, fft_size);
            remove_data(data, reduced_data, fft_size);

            fftwf_execute(plan);

            // Function computes the magnitude of each
            // complex number and creates a new array
            // mag(out, message, RESULT);

            // Here, I prepare the message that will be sent over MQTT
            Message m(MQTT_TOPIC, out, fft_result, t);
            
            pubmsg.payload = m.get_message();
            pubmsg.payloadlen = m.get_length();
            pubmsg.qos = qos;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, MQTT_TOPIC, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

            Pa_Sleep(3000);
        }

        else
        {
            Pa_Sleep(1000);
        }
    }

    err = Pa_Terminate();
    if (err != paNoError)
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}