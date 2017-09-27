#include <cmath>
#include <cstdlib>
#include <fftw3.h>
#include <portaudio.h>
#include <iostream>
#include <mosquitto.h>
#include <string>
#include <cstring>

#define SAMPLE_RATE (44100)

#define FFT_SIZE (8192)
#define RESULT (FFT_SIZE/2)

#define MQTT_TOPIC "sound"
#define MQTT_HOSTNAME "localhost"
#define MQTT_PORT 1883

// void computeSecondOrderLowPassParameters( float srate, float f, float *a, float *b )
// {
//    float a0;
//    float w0 = 2 * M_PI * f/srate;
//    float cosw0 = cos(w0);
//    float sinw0 = sin(w0);
//    //float alpha = sinw0/2;
//    float alpha = sinw0/2 * sqrt(2);

//    a0   = 1 + alpha;
//    a[0] = (-2*cosw0) / a0;
//    a[1] = (1 - alpha) / a0;
//    b[0] = ((1-cosw0)/2) / a0;
//    b[1] = ( 1-cosw0) / a0;
//    b[2] = b[0];
// }

// float processSecondOrderFilter( float x, float *mem, float *a, float *b )
// {
//     float ret = b[0] * x + b[1] * mem[0] + b[2] * mem[1]
//                          - a[0] * mem[2] - a[1] * mem[3] ;

// 		mem[1] = mem[0];
// 		mem[0] = x;
// 		mem[3] = mem[2];
// 		mem[2] = ret;

// 		return ret;
// }

std::string convert(float *arr, int size)
{
    // I am converting the float array to a
    // string so that I can send it via MQTT
    std::string ret;
    std::string temp;
    ret.append("[");
    for (int i = 0; i < size; i++)
    {
        temp = std::to_string(arr[i]);
        ret.append(temp + ", ");
    }
    ret.append("]");
    return ret;
}

void mag(fftwf_complex *out, float *data, int size)
{
    for (int i = 0; i < size/2; i++)
    {
        data[i] = sqrt(out[i][0] * out[i][0]
                        + out[i][1] * out[i][1]);
    }
}

void build_window(float *window, int size)
{
    // The following function merely builds a Welch window
    // I will apply this to the data I receive from the microphone
    for (int i = 0; i < size; i++)
    {
        int top = i - (size - 1)/2;
        int bot = (size - 1)/2;
        window[i] = 1 - pow((top/bot), 2);
    }
}

void apply_window(float *window, float *data, int size)
{
    // This applys the Welch window that was created in the baove function
    // to the input
    for (int i = 0; i < size; i++)
    {
        data[i] *= window[i];
    }
}

int main()
{
    float a[2], b[3], mem1[4], mem2[4];

    // computeSecondOrderLowPassParameters(SAMPLE_RATE, 3700, a, b);
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
        // Create the fftw plan
        fftwf_plan plan = fftwf_plan_dft_r2c_1d(FFT_SIZE, data, out, FFTW_ESTIMATE);        
        // Pa_ReadStream is a blocking call to take in mic input
        err = Pa_ReadStream(stream, data, FFT_SIZE);

        // for (int j = 0; j < FFT_SIZE; j++)
        // {
        //     data[j] = processSecondOrderFilter(data[j], mem1, a, b);
        //     data[j] = processSecondOrderFilter(data[j], mem2, a, b);
        // }

        apply_window(window, data, RESULT);
        fftwf_execute(plan);

        // Function computes the magnitude of each
        // complex number and creates a new array
        mag(out, message, RESULT);

        // What follows is a very hacky way of making an easy to parse format
        // That I can send via MQTT
        // It works but there are very likely better ways to do this
        // On the receiver's side, I do need to unpack the string
        std::string msg = convert(message, RESULT);
        const char *c = msg.c_str();
        int msgLen = strlen(c);


        ret = mosquitto_publish(
                                mosq,               // Initialized with mosquitto_lib_init
                                nullptr,            // int *mid
                                MQTT_TOPIC,         // Topic to publish to
                                msgLen,             // int payload length
                                c,                  // Message being sent
                                0,                  // Quality of Service
                                false               // Retain message
                                );
        
        // If mqtt doesn manage a succesful publish
        // There is an error and program should end
        if (ret)
        {
            exit(-1);
        }

        // write_file(out);
        // For now this is going to be 1 second but in the actual code I would like this to be 5 seconds
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