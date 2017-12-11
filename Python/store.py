import json
import paho.mqtt.client as mqtt
import pymongo
from pymongo import MongoClient

client = MongoClient()
db = client.Audio
collection = db.fft

def on_connect(client, userdata, flags, rc):
    """
    Subscribes to sound topic
    """
    client.subscribe('sound')

def on_message(client, userdata, msg):
    j = json.loads(msg.payload)
    updated_json = {
                    'time' : j['time'],
                    'mag' : j['mag'][:2048],
                    'loc' : j['loc']
                    }
    result = collection.insert_one(updated_json).inserted_id
    print(result)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()