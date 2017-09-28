#include <fftw3.h>
#include <string>
#include <cmath>

void computeSecondOrderLowPassParameters( float srate, float f, float *a, float *b )
{
   float a0;
   float w0 = 2 * M_PI * f/srate;
   float cosw0 = cos(w0);
   float sinw0 = sin(w0);
   //float alpha = sinw0/2;
   float alpha = sinw0/2 * sqrt(2);

   a0   = 1 + alpha;
   a[0] = (-2*cosw0) / a0;
   a[1] = (1 - alpha) / a0;
   b[0] = ((1-cosw0)/2) / a0;
   b[1] = ( 1-cosw0) / a0;
   b[2] = b[0];
}

float processSecondOrderFilter( float x, float *mem, float *a, float *b )
{
    float ret = b[0] * x + b[1] * mem[0] + b[2] * mem[1]
                         - a[0] * mem[2] - a[1] * mem[3] ;

		mem[1] = mem[0];
		mem[0] = x;
		mem[3] = mem[2];
		mem[2] = ret;

		return ret;
}

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