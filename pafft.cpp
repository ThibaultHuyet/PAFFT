#include <stdio.h>
#include <stdbool.h>
#include <cmath>
#include <stdlib.h>
#include <fftw3.h>
#include <portaudio.h>
#include <iostream>
#include <fstream>


#define SAMPLE_RATE (44100)
#define FFT_SIZE (8192)
#define RESULT (FFT_SIZE/2)

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

void write_file(fftwf_complex* out)
{
    std::ofstream file;
    file.open("fft.csv");

    for (int i = 0; i < FFT_SIZE; i++)
    {
        float mag = sqrt(out[i][0] * out[i][0]
                        + out[i][1] * out[i][1]
                        );

        file << mag << "," << i*5.38 <<"\n";
    }
}

int main()
{
    PaStreamParameters inputParameters;                 // Input parameters for working with PortAudio
    PaError err = 0;                                    // err is for error checking
    PaStream *stream;                                   // Stream object created by PortAudio

    // data is going to be where audio data is stored
    // data will be the input to fft
    // out will be the output
    float data[FFT_SIZE];
    fftwf_complex out[FFT_SIZE];

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
        apply_window(window, data, FFT_SIZE);
        fftwf_execute(plan);

        write_file(out);
        // For now this is going to be 1 second but in the actual code I would like this to be 5 seconds
        Pa_Sleep(1000);
    }

    err = Pa_Terminate();
    if (err != paNoError)
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    error:
        if( stream )
        {
            Pa_AbortStream( stream );
            Pa_CloseStream( stream );
        }


    return 0;
}