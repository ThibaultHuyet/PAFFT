import json
import paho.mqtt.client as mqtt
import pymongo
from pymongo import MongoClient
import numpy as np

print('storing module started')

client = MongoClient()
db = client.Audio

audio_fine   = db.fft # Data points are stored every 10 seconds
audio_coarse = db.day # Data points are stored every 10 minutes

previous_times = {}

def on_connect(client, userdata, flags, rc):
    """
    Subscribes to sound topic
    """
    client.subscribe('+/+/+/Audio')
    print('connected to sound')

def on_message(client, userdata, msg):

    global previous_times

    j = json.loads(msg.payload)
    updated_json = {
                    'time' : j['time'],
                    'complex' : j['complex'],
                    'loc' : j['loc'],
                    'latency' : j['latency']
                    }
    if updated_json['loc'] not in previous_times:
        previous_times[updated_json['loc']] = 0

    fft_result = [(r + i * 1j) for r, i in zip(updated_json['complex']['real'], updated_json['complex']['imag'])]
    S = np.absolute(fft_result)
    S = 20 * np.log10(S / np.max(S))

    if np.mean(S) < - 100:
        # From previous testing. Normal fft slices have an average around -50ish
        # Failed sensor data usually gets around -120ish
        # I just ignore all failed data points
        print('Faulty data point, passing')

    else:
        if updated_json['time'] - previous_times[updated_json['loc']] > 600: # This works out to 10 minutes
            previous_times[updated_json['loc']] = updated_json['time']
            result_coarse = audio_coarse.insert_one(updated_json).inserted_id
            print(f'Coarse result: {result_coarse} from {updated_json["loc"]}')

        result = audio_fine.insert_one(updated_json).inserted_id
        print(f'Fine result: {result} from {updated_json["loc"]}')


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# I do some testing with delays of around 10 minutes so I want some leeway
client.connect('localhost', keepalive = 700)
client.loop_forever()
