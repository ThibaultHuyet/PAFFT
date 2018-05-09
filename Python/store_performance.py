import json
import paho.mqtt.client as mqtt
import pymongo
from pymongo import MongoClient

print('storing module started')

client = MongoClient()
db = client.Performance
collection = db.fft

def on_connect(client, userdata, flags, rc):
    """
    Subscribes to sound topic
    """
    client.subscribe('Nimbus/+/+/Performance')
    print('connected to sound')

def on_message(client, userdata, msg):
    j = json.loads(msg.payload)

    total_perf = j['performance']['total']
    dockerd_perf = j['performance']['dockerd']
    container_perf = j['performance']['container']
    program_perf = j['performance']['program']

    updated_json = {'loc' : j['loc'],
                    'time': j['time'],
                    'performance' : {'total' : {'cpu': total_perf['cpu'],
                                                'mem': total_perf['mem']},
                                     'dockerd' :{'cpu':dockerd_perf['cpu'],
                                                 'mem':dockerd_perf['mem']},
                                     'container' : {'cpu':container_perf['cpu'],
                                                    'mem':container_perf['mem']},
                                     'program' : {'cpu': program_perf['cpu'],
                                                  'mem': program_perf['mem']}}}

    result = collection.insert_one(updated_json).inserted_id
    print(result)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()
