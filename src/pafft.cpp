#include <fftw3.h>                      // For performing fft
#include <portaudio.h>                  // For audio sampling
#include <mosquitto.h>                  // For sending data
#include <string>                       // Making a message to be sent
#include "lib.h"
#include "json.hpp"                     // Converting to JSON data

using json = nlohmann::json;

#define SAMPLE_RATE (44100)

#define FFT_SIZE (8192)
#define RESULT (FFT_SIZE/2)

#define MQTT_TOPIC "sound"
#define MQTT_HOSTNAME "localhost"
#define MQTT_PORT 1883

int main()
{
    // Initialize the mosquitto that will be used
    struct mosquitto *mosq = nullptr;
    mosquitto_lib_init();

    mosq = mosquitto_new(nullptr, true, nullptr);
    if (!mosq)
    {
        exit(-1);
    }

    int ret = mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
    if (ret)
    {
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
    if (err != paNoError) goto error;

    // The window array
    float window[FFT_SIZE];
    // populate the window with the Welch function
    build_window(window, FFT_SIZE);

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

    if (err != paNoError) goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;

    

    while (true)
    {
        json j; // Message to be sent
        // Create the fftw plan
        fftwf_plan plan = fftwf_plan_dft_r2c_1d(FFT_SIZE, data, out, FFTW_ESTIMATE);        
        // Pa_ReadStream is a blocking call to take in mic input
        err = Pa_ReadStream(stream, data, FFT_SIZE);

        apply_window(window, data, RESULT);
        fftwf_execute(plan);

        // Function computes the magnitude of each
        // complex number and creates a new array
        mag(out, message, RESULT);

        // What follows is a very hacky way of making an easy to parse format
        // That I can send via MQTT
        // It works but there are very likely better ways to do this
        // On the receiver's side, I do need to unpack the string
        
        j["mag"] = message;
        std::string msg = j.dump(); // Convert JSON value into string
        auto payload = msg.data(); // Convert into binary/c-string data
        int msgLen = msg.length();
        // std::string msg = convert(message, RESULT);
        // const char *c = msg.c_str();
        // int msgLen = strlen(c);


        ret = mosquitto_publish(
                                mosq,               // Initialized with mosquitto_lib_init
                                nullptr,            // int *mid
                                MQTT_TOPIC,         // Topic to publish to
                                msgLen,             // int payload length
                                payload,                  // Message being sent
                                0,                  // Quality of Service
                                false               // Retain message
                                );  

        // If mqtt doesn manage a succesful publish
        // There is an error and program should end
        if (ret)
        {
            exit(-1);
        }

        Pa_Sleep(5000);
    }

    err = Pa_Terminate();
    if (err != paNoError)
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    mosquitto_disconnect (mosq);
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();
    
    error:
        if( stream )
        {
            Pa_AbortStream( stream );
            Pa_CloseStream( stream );
        }
    mosquitto_disconnect (mosq);
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();

    return 0;
}