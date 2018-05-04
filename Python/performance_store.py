import paho.mqtt.client as mqtt
import json


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    client.subscribe("performance")

def on_message(client, userdata, msg):
    j = json.loads(msg.payload)
    print(j['performance']['total'])
    print(j['performance']['dockerd'])
    print(j['performance']['container'])
    print(j['performance']['program'])


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60)
client.loop_forever()
