import json
import paho.mqtt.client as mqtt
import pymongo
from pymongo import MongoClient

print('storing module started')

client = MongoClient()
db = client.Audio
collection = db.fft

def on_connect(client, userdata, flags, rc):
    """
    Subscribes to sound topic
    """
    client.subscribe('Nimbus/+/+/Audio')
    print('connected to sound')

def on_message(client, userdata, msg):
    j = json.loads(msg.payload)
    updated_json = {
                    'time' : j['time'],
                    'complex' : j['complex'],
                    'loc' : j['loc'],
                    'latency' : j['latency']
                    }

    result = collection.insert_one(updated_json).inserted_id
    print(result)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()
