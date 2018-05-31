import matplotlib
matplotlib.use('Agg')

import numpy as np
import matplotlib.pyplot as plt
import json
import codecs
import pymongo
from pymongo import MongoClient

client = MongoClient()
db = client.Audio
collection = db.fft

plt.clf()

try:
    # take the message received and then decode the object
    # object is received as bytes object, so decode works well
    results = collection.find({'loc' : 'Nimbus/Bot/1/Audio'}).limit(100).sort('time', pymongo.DESCENDING)
    time = []
    slices = []
    for result in results:
        fft_result = [(r + i * 1j) for r, i in zip(result['complex']['real'], result['complex']['imag'])]
        slices.append(fft_result)
        time.append(result['time'])

    spectrum = np.asarray(slices)
    S = np.absolute(spectrum)
    S = 20 * np.log(S / np.max(S))


    plt.imshow(S, cmap = 'hot', aspect = 'auto')
    plt.savefig('reconstructed'.format(time[0]))

except json.decoder.JSONDecodeError:
    print('Received unknown JSON value... Ignoring')
