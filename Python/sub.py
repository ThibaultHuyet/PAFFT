import paho.mqtt.client as mqtt
import codecs

def on_connect(client, userdata, flags, rc):
    client.subscribe('sound')

def on_message(client, userdata, msg):
    print(str(msg.payload) + "\n")


client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()