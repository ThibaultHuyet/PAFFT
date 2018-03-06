# -*- coding: utf-8 -*-
import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output
from collections import deque
import numpy as np
from pymongo import MongoClient
import pymongo

import plotly.graph_objs as go
import plotly

client = MongoClient()
db = client.Audio
collection = db.fft

time = np.arange(0, 100, 1)
frequencies = np.arange(0, 22049, 5.38330078)

results = collection.find().limit(100).sort('time', pymongo.DESCENDING)

slices = []
for result in results:
    fft_result = [(r + i * 1j) for r, i in
        zip(result['complex']['real'], result['complex']['imag'])]
    slices.append(fft_result)

spectrum = np.asarray(slices)
S = np.absolute(spectrum)
S = 20 * np.log10(S / np.max(S))

app = dash.Dash()

app.layout = html.Div([
   dcc.Graph(id = 'live-spectrograph'),
   dcc.Interval(id = 'interval-component',
                interval = 10 * 1000, # Graph updates every 10 seconds
                n_intervals = 0),

    dcc.Graph(id = 'fft-series')
])


@app.callback(Output('fft-series', 'figure'),
[Input('live-spectrograph', 'hoverData')])
def update_fft_series(hoverData):
    '''
    This is the function for allowing someone to hover over a data point
    in the spectrograph and it will then update the FFT graph with the hovered data
    '''
    trace = go.Scatter(x = frequencies,
                    y = S[hoverData['points'][0]['x']].T,
                    mode = 'lines')

    layout = go.Layout(title = 'FFT of point',
                        xaxis = dict(title = 'Frequency (Hz)',
                                    type = 'log'),
                        yaxis = dict(title = 'dB'))

    return go.Figure(data = [trace], layout = layout)


@app.callback(Output('live-spectrograph', 'figure'),
            [Input('interval-component', 'n_intervals')])
def update_spectrogram(n):
    '''
    Function for keeping the spectrograph to updated live
    It makes a call to a MongoDB database.
    '''
    results = collection.find().limit(100).sort('time', pymongo.DESCENDING)

    slices = []
    for result in results:
        fft_result = [(r + i * 1j) for r, i in
        zip(result['complex']['real'], result['complex']['imag'])]
        slices.append(fft_result)

    spectrum = np.asarray(slices)
    S = np.absolute(spectrum)
    S = 20 * np.log10(S / np.max(S))

    trace = go.Heatmap(x = time, y = frequencies, z = S.T)
    layout = go.Layout(title = 'Spectrogram of Microphone',
                        xaxis = dict(title = 'Time Passed(s)')
                        yaxis = dict(title = 'Frequencies (kHz)'))

    return go.Figure(data = [trace], layout = layout)


if __name__ == '__main__':
    app.run_server(host = '0.0.0.0', debug=True)