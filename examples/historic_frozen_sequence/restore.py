import h5py
import numpy as np
from QPot import Chunked
from prrng import pcg32

with h5py.File("output.h5", "r") as data:
    chunks = data["/restore/chunks"][...]
    state = data["/restore/state"][...]
    xmin = data["/restore/xmin"][...]
    initstate = data["/restore/initstate"][...]
    initseq = data["/restore/initseq"][...]
    x = data["/text/x"][...]
    yl = data["/text/yl"][...]
    yr = data["/text/yr"][...]

yl_ = np.empty_like(yl)
yr_ = np.empty_like(yr)

generator = pcg32(initstate=initstate, initseq=initseq)
chunk = Chunked(0.0, [-1, 0, 1])

for i, xi in enumerate(x):

    chunk.set_x(xi)

    ichunk = np.argmax(xi < xmin) - 1

    generator.restore(state[ichunk])
    dy = generator.random([int(chunks[ichunk + 1] + 1 - chunks[ichunk])])

    dy[0] = xmin[ichunk]

    chunk.set_y(chunks[ichunk], np.cumsum(dy))

    yl_[i] = chunk.yleft()
    yr_[i] = chunk.yright()

assert np.allclose(yl, yl_)
assert np.allclose(yr, yr_)
