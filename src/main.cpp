#include <armadillo>
#include <fftw3.h>                      // For performing fft
#include "Portaudio.hpp"
#include <ctime>
#include "lib.hpp"
#include "Message.hpp"
#include "MQTT.hpp"
#include "sigpack.h"

using namespace sp;

int main()
{
    std::string address = "localhost";
    std::string client  = "Nimbus1";
    std::string loc     = "filt";

    int fft_size    = 8192;
    int sample_rate = 44100;
    int fft_result  = (fft_size) / 2;
    int qos         = 0;


    arma::vec e(fft_size, arma::fill::zeros);
    arma::vec d(fft_size, arma::fill::zeros);
    arma::vec y(fft_size, arma::fill::zeros);

    FIR_filt<double, double, double> G;
    FIR_filt<double, double, double> Ghat;
    arma::vec b = "-0.2 -0.1 0.1 0.3 0.7";
    G.set_coeffs(b);
    Ghat.setup_rls(5, 0.95, 50);


    // Initialize the mosquitto client that will be used
    MQTT mqtt(address, client);

    // data is going to be where audio data is stored
    // data will be the input to fft
    // out will be the output
    float         data[fft_size];
    fftw_complex  out[fft_size / 2];
    float         message[fft_size/2];
    float         *filtered;

    // Initialize PortAudio
    Portaudio PA;
    PA.open_stream(fft_size, sample_rate, paClipOff);
    
    int   pt = 0;
    float lat = 0; // Latency variable
    while (true)
    {
        int t = time(nullptr);
        if (t - pt > 4 && t % 5 == 0)
        {    
            pt = t;
	    PA.start_stream();
            // Create the fftw plan
            fftw_plan plan = fftw_plan_dft_r2c_1d(fft_size, y.memptr(), out, FFTW_ESTIMATE);        
            
            // Pa_ReadStream is a blocking call to take in mic input
            PA.read_stream(data, fft_size);

            // Have to do this really dumb solution as initialization attempts
            // have caused segmentation faults saying the size of data is 
            // incompatible with x
            arma::vec x(fft_size, arma::fill::zeros);
            for (int n = 0; n < fft_size; n++)
            {
                x(n) = data[n];
            }

            d = G.filter(x);

            for (int n = 0; n < fft_size; n++)
            {
                y(n) = Ghat(x(n));
                e(n) = d(n) - y(n);

                Ghat.rls_adapt(e(n));
            }

            fftw_execute(plan);
	    PA.stop_stream();
            // Here, I prepare the message that will be sent over MQTT
            Message m(loc, out, fft_result, t, lat);
            
            clock_t time_before_publish = clock();
            mqtt.publish_message(m, loc, qos);
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
    return 0;
}
