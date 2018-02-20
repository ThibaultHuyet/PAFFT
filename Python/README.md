# Sound Sensor Server Stuff

The Python side of this repo is intended to be run on an AWS EC2 instance. audio_check.py is intended for debugging purposes, that is to say that it is intended to check that the audio data being received is usable.
store.py is intended to store the audio data received into a database.


## store.py
### Libraries
* [MongoDB](https://www.mongodb.com/) (Subject to change)
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
