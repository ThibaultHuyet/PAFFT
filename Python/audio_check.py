import matplotlib
matplotlib.use('Agg')

import paho.mqtt.client as mqtt
import numpy as np
import matplotlib.pyplot as plt
import json
import codecs

def on_connect(client, userdata, flags, rc):
    print('connected with result {0}'.format(rc))
    client.subscribe('sound')

def on_message(client, userdata, msg):
    # clear the plot window as it will be used multiple times
    plt.clf()

    try:
        # take the message received and then decode the object
        # object is received as bytes object, so decode works well
        reader = codecs.decode(msg.payload)
        j = json.loads(reader)

        # cretae an array of tupls that can be inserted into the ifft function
        a = [(r, i*1j) for r, i in zip(j['complex']['real'], j['complex']['imag'])]
        print(a)
        print(j['time'])
        s = np.fft.ifft(a)

        # plot the results
        t = len(s.real)
        points = np.arange(t)
        plt.plot(points, s.real, 'b-', points, s.imag, 'ro')
        plt.savefig('reconstructed_{0}'.format(j['time']))

    except json.decoder.JSONDecodeError:
        print('Received unknown JSON value... Ignoring')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost', 1883, 60)
client.loop_forever()
