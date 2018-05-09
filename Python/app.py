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
import json
from bson import json_util

client = MongoClient()
db = client.Audio
collection = db.fft

frequencies = np.arange(0, 22049, 5.38330078)

app = dash.Dash()

app.layout = html.Div([
   dcc.Dropdown(id = 'loc-dropdown',
                options = [{'label': 'Top Floor Nimbus 1', 'value': 'Nimbus/Top/1/Audio'},
                           {'label': 'Top Floor Nimbus 2', 'value': 'Nimbus/Top/2/Audio'},
                           {'label': 'Bottom Floor Nimbus 1', 'value': 'Nimbus/Bot/1/Audio'},
                           {'label': 'Bottom Floor Nimbus 2', 'value': 'Nimbus/Bot/2/Audio'}],
                value = 'Nimbus/Top/1/Audio'),
   dcc.Graph(id = 'live-spectrograph'),
   dcc.Interval(id = 'interval-component',
                interval = 10 * 1000, # Graph updates every 10 seconds
                n_intervals = 0),

    dcc.Graph(id = 'fft-series'),
    dcc.Graph(id = 'latency'),
    dcc.Interval(id = 'latency-interval-component',
                interval = 10*1000,
                n_intervals = 0)
])


@app.callback(Output('latency', 'figure'),
[Input('latency-interval-component', 'n_intervals'),
Input('loc-dropdown', 'value')])
def update_latency(n, dropdown):

    results = collection.find({'loc': dropdown},
    {'_id': False, 'real': False, 'complex': False}).limit(100).sort('time', pymongo.DESCENDING)

    time = []
    lat = []
    for result in results:
        time.append(result['time'])
        lat.append(result['latency'])

    trace = go.Scatter(x = time,
                        y = lat,
                        mode = 'lines')

    layout = go.Layout(title = 'Latency',
                        xaxis = dict(title = 'Time'),
                        yaxis = dict(title = 'Latency'))

    return go.Figure(data = [trace], layout = layout)

@app.callback(Output('fft-series', 'figure'),
[Input('live-spectrograph', 'hoverData'),
Input('loc-dropdown', 'value')])
def update_fft_series(hoverData, dropdown):
    '''
    This is the function for allowing someone to hover over a data point
    in the spectrograph and it will then update the FFT graph with the hovered data
    '''
    result = collection.find_one({'time' : hoverData['points'][0]['x'],
                               'loc' : dropdown})

    fft = json.loads(result, default = json_util.default)
    fft = [(r + i*1j) for r, i in zip(fft['complex']['real'], fft['complex']['imag'])]
    
    trace = go.Scatter(x = frequencies,
                    y = fft,
                    mode = 'lines')

    layout = go.Layout(title = 'FFT of point',
                        xaxis = dict(title = 'Frequency (Hz)',
                                    type = 'log'),
                        yaxis = dict(title = 'dB'))

    return go.Figure(data = [trace], layout = layout)


@app.callback(Output('live-spectrograph', 'figure'),
            [Input('interval-component', 'n_intervals'),
             Input('loc-dropdown', 'value')])
def update_spectrogram(n, dropdown):
    '''
    Function for keeping the spectrograph to updated live
    It makes a call to a MongoDB database.
    '''
    results = collection.find({'loc':dropdown}).limit(100).sort('time', pymongo.DESCENDING)
    
    time = []
    slices = []
    for result in results:
        fft_result = [(r + i * 1j) for r, i in
        zip(result['complex']['real'], result['complex']['imag'])]
        slices.append(fft_result)
        time.append(result['time'])

    spectrum = np.asarray(slices)
    S = np.absolute(spectrum)
    S = 20 * np.log10(S / np.max(S))

    trace = go.Heatmap(x = time, y = frequencies, z = S.T)
    layout = go.Layout(title = 'Spectrogram of Microphone',
                        xaxis = dict(title = 'Unix Time'),
                        yaxis = dict(title = 'Frequencies (kHz)'))

    return go.Figure(data = [trace], layout = layout)


if __name__ == '__main__':
    app.run_server(host = '0.0.0.0', debug=True)
