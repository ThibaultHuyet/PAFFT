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
from bson.json_util import CANONICAL_JSON_OPTIONS, loads

client = MongoClient()
db = client.Audio

collection = db.fft
power = db.power

frequencies = np.arange(0, 22049, 5.38330078)

app = dash.Dash()

app.layout = html.Div([
   html.Div([
      html.H1('Nimbus Noise Levels', style = {'text-align': 'center'})
           ]),
   html.Div([
      html.Div([
         dcc.Dropdown(id = 'loc-dropdown',
                options = [{'label': 'Nimbus Top Floor 1', 'value': 'Nimbus/Top/1/Audio'},
                           {'label': 'Nimbus Top Floor 2', 'value': 'Nimbus/Top/2/Audio'},
                           {'label': 'Nimbus Bot Floor 1', 'value': 'Nimbus/Bot/1/Audio'},
                           {'label': 'Nimbus Bot Floor 2', 'value': 'Nimbus/Bot/2/Audio'}],
                value = 'Nimbus/Top/1/Audio',
		clearable = False)
              ],
         style = {'width': '20%',
		  'margin-left': 100}),

      html.Div([
         html.Div([
            dcc.Graph(id = 'live-spectrograph'),
            dcc.Interval(id = 'interval-component',
                interval = 10 * 1000, # Graph updates every 10 seconds
                n_intervals = 0)
                 ],
            style = {'display':'inline-block',
		     'width' : 700}),
         html.Div([
            dcc.Graph(id = 'fft-series')
              ],
            style = {'display':'inline-block',
		     'width': 500})
	     ],
         style = {'display':'inline-block',
		  'height': 400,
		  'margin-left' : 20,
		  'margin-right': 0}),
       html.Div([
	  html.Div([
             dcc.Graph(id = 'latency'),
             dcc.Interval(id = 'latency-interval-component',
                interval = 10*1000,
                n_intervals = 0)],
             style = {'display' : 'inline-block',
                      'height' : 400,
                      'width' : 500}),

          html.Div([
             dcc.Graph(id = 'live-power'),
             dcc.Interval(id = 'power-interval',
                 interval = 10 * 1000,
                 n_intervals = 0)],
             style = {'display' : 'inline-block',
                      'height' : 400,
                      'width' : 500,
                      'margin-left' : 50,
                      'margin-right' : 50})
         ]),

       html.Div([
          html.Div([
             dcc.Dropdown(id = 'performance-dropdown',
		 options = [{'label':'Memory', 'value': 'mem'},
			    {'label':'CPU', 'value': 'cpu'}],
		 value = 'cpu',
                 clearable = False),
             dcc.Dropdown(id = 'loc-perf-dropdown',
		 options = [{'label': 'Nimbus Top Floor 1', 'value': 'Nimbus/Top/1'},
			    {'label': 'Nimbus Top Floor 2', 'value':'Nimbus/Top/2'},
			    {'label':'Nimbus Bot Floor 1', 'value':'Nimbus/Bot/1'},
			    {'label': 'Nimbus Bot Floor 2', 'value':'Nimbus/Bot/2'}],
		 value = 'Nimbus/Top/1',
                 clearable = False),
             dcc.Dropdown(id = 'program-dropdown',
		 options = [{'label': 'Total', 'value': 'total'},
			    {'label': 'Shell', 'value': 'shell'},
			    {'label': 'Program', 'value': 'program'},
			    {'label': 'Docker', 'value' : 'dockerd'},
			    {'label': 'Container', 'value': 'container'}],
		 value = 'total',
                 clearable = False)],
             style = {'display' : 'inline-block',
                      'width' : '20%'})]),
])
],
style = {'backgroundColor' : '#808080' })

@app.callback(Output('live-power', 'figure'),
[Input('power-interval', 'n_intervals'),
Input('performance-dropdown', 'value'),
Input('loc-perf-dropdown', 'value'),
Input('program-dropdown', 'value')])
def update_power(n, perf, loc, prog):
    results = power.find({'loc': loc}).limit(100).sort('time', pymongo.DESCENDING)

    time = []
    total = []
    for result in results:
        time.append(result['time'])
        total.append(result['performance'][prog][perf])

    trace = go.Scatter(x = time,
                       y = total,
                       mode = 'lines')

    layout = go.Layout(title = 'Power',
                       xaxis = dict(title = 'Time',
                                    tickformat = 'f'),
                       yaxis = dict(title = '%CPU'))

    return go.Figure(data = [trace], layout = layout)

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
        lat.append(result['latency']* 1000)

    trace = go.Scatter(x = time,
                        y = lat,
                        mode = 'lines')

    layout = go.Layout(title = 'Latency',
                        xaxis = dict(title = 'Time',
				     tickformat = 'f'),
                        yaxis = dict(title = 'Latency'))

    return go.Figure(data = [trace], layout = layout)

@app.callback(Output('fft-series', 'figure'),
[Input('live-spectrograph', 'clickData'),
Input('loc-dropdown', 'value')])
def update_fft_series(clickData, dropdown):
    '''
    This is the function for allowing someone to hover over a data point
    in the spectrograph and it will then update the FFT graph with the hovered data
    '''
    time = clickData['points'][0]['x']
    fft_slice = collection.find_one({'time' : time,
                               'loc' : dropdown})

    fft_graph = [np.absolute(r + i * 1j) for r, i in zip(fft_slice['complex']['real'], fft_slice['complex']['imag'])]
    fft_graph = 20 * np.log(fft_graph/np.max(fft_graph))
    
    trace = go.Scatter(x = frequencies,
                    y = fft_graph,
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
    results = collection.find({'loc': 'Nimbus/Bot/1/Audio'}).limit(100).sort('time', pymongo.DESCENDING)
    
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

    trace = go.Heatmap(x = time, y = frequencies, z = S.T,
		       hoverinfo = 'x+y', showscale = False)
    layout = go.Layout(title = 'Spectrogram of Microphone',
                        xaxis = dict(title = 'Unix Time',
				     tickformat = 'f'),
                        yaxis = dict(title = 'Frequencies (kHz)'))
    return go.Figure(data = [trace], layout = layout)


if __name__ == '__main__':
    app.run_server(host = '0.0.0.0', debug=True)
