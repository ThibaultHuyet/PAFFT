import json
import matplotlib.pyplot as plt
import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, rc):
    """
    Subscribes to sound topic
    """
    client.subscribe('sound')

def on_message(client, userdata, msg):
    """
    Deals with what happens when receiving a message
    I want this to push this to a database
    """
    j = json.loads(msg.payload)
    plt.plot(j['mag'])
    plt.show()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()