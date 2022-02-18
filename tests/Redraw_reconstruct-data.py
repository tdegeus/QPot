import time

import h5py
import numpy as np
import QPot

seed = int(time.time())
x = 5.0 * np.random.random(30) - 2.5
n = 100
ntotal = 40
nbuffer = 10
noffset = 5

with h5py.File("Redraw_reconstruct-data.h5", "w") as data:

    data["/seed"] = seed
    data["/x"] = x
    data["/n"] = n
    data["/ntotal"] = ntotal
    data["/nbuffer"] = nbuffer
    data["/noffset"] = noffset

    rand = QPot.random.RandList()

    QPot.random.seed(seed)
    this = QPot.RedrawList(x, rand, ntotal, nbuffer, noffset)

    redraw = np.zeros((x.size, n), dtype=np.int32)

    for i in range(n):
        this.setPosition(float(i) * x)
        redraw[:, i] = this.currentRedraw()

    data["/redraw"] = redraw
    data["/raw_pos"] = this.raw_pos()
    data["/currentYieldLeft"] = this.currentYieldLeft()
    data["/currentYieldRight"] = this.currentYieldRight()
    data["/currentIndex"] = this.currentIndex()
