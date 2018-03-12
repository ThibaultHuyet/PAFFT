import numpy as np
from pymongo import MongoClient
import pymongo

time = np.arange(-500, 0, 5)
frequencies = np.arange(0, 22049, 5.38330078)

client = MongoClient()
db = client.Audio
collection = db.fft

results = collection.find().limit(100).sort('time', pymongo.DESCENDING)

slices = []
for result in results:
    fft_result = [(r + i * 1j) for r, i in
        zip(result['complex']['real'], result['complex']['imag'])]
    slices.append(fft_result)

spectrum = np.array(slices)
S = np.absolute(spectrum)
S = 20 * np.log10(S / np.max(S))

print(len(time))
print(len(frequencies))
print(len(S[0]))