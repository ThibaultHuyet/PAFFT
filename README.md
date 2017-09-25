# PAFFT
This program takes in Audio from PortAudio and then applies a FFT to the audio input. The audio input will then be sent via MQTT to be stored in a database (?) and graphed.

## Libraries Used
### c/c++ Libraries
1. ALSA
2. PortAudio
3. fftw
4. MQTT (Eventually)

### Python
1. Plotly
2. MQTT (Eventually)

## To Do
1. I need to choose either c or c++ and stick to the style. The code right now is a cross between the two and quite ugly.
2. Add MQTT to the program so that I don't do the hacky workaround of writing to a file.
3. Add this to a Raspberry Pi.
4. Test this in a Docker container.
5. Better graphing.
