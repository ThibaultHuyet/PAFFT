#include <fftw3.h>
#include <string>
#include <cmath>

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