# -*- coding: utf-8 -*-
import dash
import dash_core_components as dcc
import dash_html_components as html

import numpy as np
from pymongo import MongoClient
import plotly.graph_objs as go

client = MongoClient()
db = client.Audio
collection = db.fft

results = collection.find().limit(100).sort('time', -1)

time = np.arange(-500, 0, 5)
frequencies = np.arange(0, 22050, 5.38330078)

slices = []
for result in results:
    fft_result = [(r + i * 1j) for r, i in
    zip(result['complex']['real'], result['complex']['imag'])]
    slices.append(fft_result)

spectrum = np.array(slices)
S = np.absolute(spectrum)
S = 20 * np.log10(S / np.max(S))

app = dash.Dash()

app.layout = html.Div(children=[
   html.H1(children='Hello Dash'),
   html.Div(children='''
        Dash: A web application framework for Python.
    '''),
   dcc.Graph(
        id='example-graph',
        figure = go.Figure(
            data = [go.Heatmap(
                x = time, y = frequencies, z = S.T,
                colorscale = 'Viridis',
                )],
            layout = {'xaxis' : dict(title = 'Time (s)'),
                    'yaxis' : dict(title = 'Frequencies (kHz)')}
        )
        # figure={
        #     'data': [
        #         {'x': time,
        #         'y': frequencies,
        #         'z' : S.T,
        #         'type': 'heatmap',
        #         'xaxis' : 'Time (s)',
        #         'yaxis' : 'Frequencies (kHz)'},
        #     ],
        #     'layout': {
        #         'title': 'Dash Data Visualization'
        #     }
        # }
    )
])

if __name__ == '__main__':
    app.run_server(host = '0.0.0.0', debug=True)