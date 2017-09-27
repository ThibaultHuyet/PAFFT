# PAFFT
This program takes in Audio from PortAudio and then applies a FFT to the audio input. The audio input will then be sent via MQTT to be stored in a database (?) and graphed.

## Libraries Used
### c/c++ Libraries
1. [PortAudio](http://www.portaudio.com/)
2. [fftw](http://www.fftw.org/)
3. [MQTT](https://mosquitto.org/)

### Python
1. [Plotly](https://plot.ly/)
2. [MQTT](http://www.eclipse.org/paho/)

## To Do
1. I need to choose either c or c++ and stick to the style. The code right now is a cross between the two and quite ugly.
2. Add this to a Raspberry Pi.
3. Test this in a Docker container.
4. Better graphing.
5. Maybe store this in a MongoDB database
6. Run Python side on AWS

## Bugs
1. Something about the fft graphing isn't working properly. PRoblem is either on the magnitude function or in the actual graphing. Must research/discuss on how exactly the output of the fft transfrom should be interpreted. Look into how other people have graphed fft transforms and if it has been done in Plotly.