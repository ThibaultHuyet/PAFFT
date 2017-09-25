import plotly.plotly as py
import plotly.graph_objs as go
import math

mag = []
frequency = []

with open('fft.txt', 'r') as file:
    for line in file:
        temp = line.split(',')
        mag.append(temp[0])
        frequency.append(float(temp[1].rstrip()))

for i, value in enumerate(mag):
    if (math.isnan(float(value))):
        mag[i] = 0
    elif (math.isinf(float(value))):
        mag[i] = 0
    elif (float(value) > 1000):
        mag[i] = 0
    else:
        mag[i] = float(value)

trace = go.Scatter(
                    y = mag
                    )

data = [trace]
py.plot(data, filename = 'basic_line')