#include <fftw3.h>                      // For performing fft
#include <portaudio.h>                  // For audio sampling
#include "MQTTClient.h"                  // For sending data
#include <string>                       // Making a message to be sent
#include <ctime>
#include <iostream>
#include "lib.h"
#include "Message.hpp"

#define SAMPLE_RATE (44100)

#define FFT_SIZE (8192)
#define RESULT (FFT_SIZE/2)

#define QOS 0
#define MQTT_TOPIC "sound"
#define MQTT_HOSTNAME "ec2-54-229-155-118.eu-west-1.compute.amazonaws.com"
#define MQTT_PORT 1883
#define CLIENTID "Thibault"
#define TIMEOUT 2000L
int main()
{
    /*
    The following files are used if AWS IoT
    is the broker being connected to.
    Be sure to download the capath.
    Keyfile is the private key.
    Sometimes AWS IoT doesn connect instantly and takes a
    while for it fully connect.
    */
    std::string cafile = "tls/rootCA.pem";
    std::string cert = "tls/bc158e9240-certificate.pem.crt";
    std::string key = "tls/bc158e9240-private.pem.key";

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
    float data[FFT_SIZE];
    fftwf_complex out[FFT_SIZE/2];
    float message[FFT_SIZE/2];

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
                        SAMPLE_RATE,            // sample rate
                        FFT_SIZE,               // frames per buffer
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
            fftwf_plan plan = fftwf_plan_dft_r2c_1d(FFT_SIZE, data, out, FFTW_ESTIMATE);        
            
            // Pa_ReadStream is a blocking call to take in mic input
            err = Pa_ReadStream(stream, data, FFT_SIZE);

            fftwf_execute(plan);

            // Function computes the magnitude of each
            // complex number and creates a new array
            mag(out, message, RESULT);

            // Here, I prepare the message that will be sent over MQTT
            Message m(MQTT_TOPIC, message, RESULT, t);
            
            pubmsg.payload = m.get_message();
            pubmsg.payloadlen = m.get_length();
            pubmsg.qos = QOS;
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