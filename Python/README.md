# Sound Sensor Server Stuff

The Python side of this repo is intended to be run on an AWS EC2 instance. audio_check.py is intended for debugging purposes, that is to say that it is intended to check that the audio data being received is usable.
store.py is intended to store the audio data received into a database.


## store.py
### Libraries
* [MongoDB](https://www.mongodb.com/) (Might be changed)
* [Paho MQTT](https://www.eclipse.org/paho/clients/python/)

### To DO
* Do real test on AWS
* consider other databases. MongoDB works fine but I somewhat want to try out other databases
* Consider whether it is possible to indicate when a failure has happened/reset RPi 

## audio_check.py
### Libraries
* [Paho MQTT](https://www.eclipse.org/paho/clients/python/)
* [Matplotlib](https://matplotlib.org/)
* [Numpy](http://www.numpy.org/)

### To Do
* Consider whether it is possible to indicate when a failure has happened/reset RPi
* Add Images received from tests to this README

## app.py
### Libraries
* [Plotly Dash](https://plot.ly/products/dash/)
* [MongoDB](https://www.mongodb.com/) (Might be changed)

### Graphs
#### Computer's Microhphone
![alt text](https://imgur.com/Tmr4ogk.png "5k tone generator")

![alt text](https://imgur.com/HREHQtK.png "14k tone generator")

![alt text](https://imgur.com/TByTIiu.png "700 tone generator")

#### Raspberry Pi's microphone
![alt text](https://imgur.com/kgMBGSR.png "200 and 450 tone generator")

![alt text](https://imgur.com/kgMBGSR.png "5k tone generator")

![alt text](https://imgur.com/eSa1vFE.png "10k tone generator")