import plotly.plotly as py
import plotly.graph_objs as go
import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    client.subscribe('speech')

def on_message(client, userdata, msg):
    temp = str(msg.payload)
    temp = temp[3:-4]
    result = temp.split(',')
    n = len(result)

    trace = go.Scatter(
                    x = n,
                    y = result
                        )
    
    data = [trace]
    fig = go.Figure(data = data)
    py.plot(fig, filename = "Speech wave")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect('localhost')
client.loop_forever()