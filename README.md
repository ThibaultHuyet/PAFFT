# PAFFT
This program takes in Audio from PortAudio and then applies a FFT to the audio input. The audio input will then be sent via MQTT to be stored in a database and graphed.

## Build && run Docker
docker build -t sound_sensor .

docker run --privileged -dp 1883 sound_sensor

## Libraries Used
### c/c++ Libraries
1. [PortAudio](http://www.portaudio.com/)
2. [fftw](http://www.fftw.org/)
3. [MQTT](https://mosquitto.org/)

### Python
1. [Plotly](https://plot.ly/)
2. [MQTT](http://www.eclipse.org/paho/)

## Circuit Diagram
![Image provided by Adafruit](https://cdn-learn.adafruit.com/assets/assets/000/039/636/original/sensors_pi_i2s_bb.png?1487800378)

[Taken from Adafruit](https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/raspberry-pi-wiring-and-test#)

[Drivers from here](https://github.com/PaulCreaser/rpi-i2s-audio)


## To Do
1. Improve graphs
2. Add synchronization
3. Commit changes that allow for sending to AWS
4. Graph data of multiple devices running synchronously
2. Get data
3. Perform tests

## Bugs
1. There seems to be an issue with Jackd server just not starting after a while of using Docker or program. To fix, you can just use the command jackd -d dummy. jackd -d alsa has bugs for some reason.
2. Also must use pulseaudio -D
3. portaudio has issues getting defaults. Need to edit config files. [Solution](https://www.raspberrypi.org/forums/viewtopic.php?t=136974)

## Graphs
### Graph of fft output
Note that this is restricted between 50 Hz and 3000 Hz

Graphs made on 28th September
<div>
    <a href="https://plot.ly/~ThibaultHuyet/94/?share_key=s3pS5zEYxPadnJVijDwscc" target="_blank" title="plot from API (37)" style="display: block; text-align: center;"><img src="https://plot.ly/~ThibaultHuyet/94.png?share_key=s3pS5zEYxPadnJVijDwscc" alt="plot from API (37)" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plot.ly/404.png';" /></a>
</div>

<div>
    <a href="https://plot.ly/~ThibaultHuyet/98/?share_key=uPQ5DZql63sfwM3chibHGj" target="_blank" title="plot from API (39)" style="display: block; text-align: center;"><img src="https://plot.ly/~ThibaultHuyet/98.png?share_key=uPQ5DZql63sfwM3chibHGj" alt="plot from API (39)" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plot.ly/404.png';" /></a>
</div>

<div>
    <a href="https://plot.ly/~ThibaultHuyet/104/?share_key=DSSYaQXPpjh5DoNWLZgk4l" target="_blank" title="plot from API (42)" style="display: block; text-align: center;"><img src="https://plot.ly/~ThibaultHuyet/104.png?share_key=DSSYaQXPpjh5DoNWLZgk4l" alt="plot from API (42)" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plot.ly/404.png';" /></a>
</div>

<div>
    <a href="https://plot.ly/~ThibaultHuyet/86/?share_key=HjyaDeRJK5N8PPrMpTJBp4" target="_blank" title="plot from API (33)" style="display: block; text-align: center;"><img src="https://plot.ly/~ThibaultHuyet/86.png?share_key=HjyaDeRJK5N8PPrMpTJBp4" alt="plot from API (33)" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plot.ly/404.png';" /></a>
</div>

<div>
    <a href="https://plot.ly/~ThibaultHuyet/84/?share_key=Xw7A2sYUfgHJas8emwSOOy" target="_blank" title="plot from API (32)" style="display: block; text-align: center;"><img src="https://plot.ly/~ThibaultHuyet/84.png?share_key=Xw7A2sYUfgHJas8emwSOOy" alt="plot from API (32)" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plot.ly/404.png';" /></a>
</div>
