import json
import matplotlib.pyplot as plt
import numpy as np
import paho.mqtt.client as mqtt

samplefreq = 44100

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
    # Determine bin frequency sizes
    binnum = len(j['mag'])
    reso = samplefreq / binnum
    x_temp = np.arange(0, binnum / 2)
    x = [x * reso for x in x_temp]

    # normalize the results
    maxj = max(j['mag'])
    normj = [x / maxj for x in j['mag']]
    
    # plot the results
    plt.plot(x, normj[:2048])
    plt.xscale('log')
    plt.show()
    
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()