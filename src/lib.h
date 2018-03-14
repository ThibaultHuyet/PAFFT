#pragma once

#include <fftw3.h>
#include <string>
#include <cmath>

void mag(fftwf_complex *out, float *data, int size)
{
    for (int i = 0; i < size/2; i++)
    {
        data[i] = sqrt(out[i][0] * out[i][0]
                        + out[i][1] * out[i][1]);
    }
}

void remove_data(float *original, float *reduced_data, int size)
{
    // There seems to be an issue in the raspberry pi where the first few points
    // are on a completely different scale.
    // Take a look at some of the images to see what it does to the spectrograph
    // This function aims to get rid of these useless data points and only keep
    // the data that seems to be useful
    for (int i = 0; i < size / 2; i++)
        {
            if (i <= 200)
                continue;
            else
                reduced_data[i] = original[i];
        }
}