#include <fftw3.h>                      // For performing fft
#include <portaudio.h>                  // For audio sampling
#include <mosquitto.h>                  // For sending data
#include <string>                       // Making a message to be sent
#include <ctime>
#include <iostream>
#include "lib.h"
#include "Message.hpp"

#define SAMPLE_RATE (44100)

#define FFT_SIZE (8192)
#define RESULT (FFT_SIZE/2)

#define MQTT_TOPIC "sound"
#define MQTT_HOSTNAME "a3va7bb54859zr.iot.eu-west-1.amazonaws.com"
#define MQTT_PORT 8883

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

    // Initialize the mosquitto that will be used
    struct mosquitto *mosq = nullptr;
    mosquitto_lib_init();

    mosq = mosquitto_new(nullptr, true, nullptr);
    if (!mosq)
    {
        exit(-1);
    }

    int ret = 0;

    ret = mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 60);
    if (ret)
    {
        std::cout << "Could not connect to server\n";
        exit(-1);
    }

    ret = mosquitto_tls_set(mosq,
                            cafile.data(),
                            nullptr,
                            cert.data(),
                            key.data(),
                            nullptr
                            );

    if (ret)
    {
        std::cout << mosquitto_strerror(ret) << std::endl;
        std::cout << "Could not authenticate\n";
        exit(-1);
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
            ret = mosquitto_publish(
                                    mosq,               // Initialized with mosquitto_lib_init
                                    nullptr,            // int *mid
                                    MQTT_TOPIC,         // Topic to publish to
                                    m.get_length(),     // int payload length
                                    m.get_message(),    // Message being sent
                                    1,                  // Quality of Service
                                    false               // Retain message
                                    );  
            
            if (ret == 14)
            {
                std::cout << "Reconnecting...\n";
                ret = mosquitto_reconnect(mosq);
            }
            
            // If mqtt doesn manage a succesful publish
            // There is an error and program should end
            else if (ret)
            {
                std::cout << mosquitto_strerror(ret);
            }
            Pa_Sleep(1000);
        }

        else
        {
            Pa_Sleep(1000);
        }
    }

    err = Pa_Terminate();
    if (err != paNoError)
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    mosquitto_disconnect (mosq);
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();

    return 0;
}