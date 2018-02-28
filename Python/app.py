# -*- coding: utf-8 -*-
import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output

import numpy as np
from pymongo import MongoClient
import plotly.graph_objs as go
import plotly

time = np.arange(-500, 0, 5)
frequencies = np.arange(0, 22050, 5.38330078)

app = dash.Dash()

app.layout = html.Div(children = [
   dcc.Graph(id = 'live-spectrograph'),
   dcc.Interval(id = 'interval-component',
                interval = 10 * 1000,
                n_intervals = 0)
])

@app.callback(Output('live-spectrograph', 'figure'),
            [Input('interval-component', 'n_intervals')])
def update_spectrogram(n):
    client = MongoClient()
    db = client.Audio
    collection = db.fft

    results = collection.find().limit(100).sort('time', -1)

    slices = []
    for result in results:
        fft_result = [(r + i * 1j) for r, i in
        zip(result['complex']['real'], result['complex']['imag'])]
        slices.append(fft_result)

    spectrum = np.array(slices)
    S = np.absolute(spectrum)
    S = 20 * np.log10(S / np.max(S))

    trace = go.Heatmap(x = time, y = frequencies, z = S.T)
    layout = go.Layout(title = 'Spectrogram of Microphone',
                        xaxis = dict(title = 'Time (s)'),
                        yaxis = dict(title = 'Frequencies (kHz)'))

    return go.Figure(data = [trace], layout = layout)


if __name__ == '__main__':
    app.run_server(host = '0.0.0.0', debug=True)