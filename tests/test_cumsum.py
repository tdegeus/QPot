import faulthandler
import unittest

import numpy as np
import prrng
import QPot

faulthandler.enable()


class Test_cumsum(unittest.TestCase):
    """
    Test QPot.lower_bound*
    """

    def test_cumsum_simple(self):

        da = np.random.random(1100)
        a = np.cumsum(da)

        n = 10
        chunk = np.copy(a[: n + 2])
        nchunk = chunk.shape[-1]
        istart = 0

        for i in range(20):
            chunk = QPot.cumsum_chunk(chunk, da[istart + nchunk : istart + nchunk + n], n)
            istart += n
            self.assertTrue(np.allclose(chunk, a[istart : istart + nchunk]))

        for i in range(18):
            chunk = QPot.cumsum_chunk(chunk, da[istart - n : istart + 1], -n)
            istart -= n
            self.assertTrue(np.allclose(chunk, a[istart : istart + nchunk]))

    def test_example_prrng(self):

        N = 3

        initstate = np.arange(N)
        initseq = np.zeros(N)
        generators = prrng.pcg32_array(initstate, initseq)

        n = 30
        nbuffer = 4
        state = generators.state()

        # generate full list of "a" for checking
        offset = 1.0
        da_ref = generators.random([n * 100])
        a = np.cumsum(da_ref, 1) - offset

        # generate some history for "x" (first increasing then decreasing) in "a"
        x = np.cumsum(0.4 * np.abs(np.random.random(N * 1000)).reshape(N, -1), axis=1)
        d = np.cumsum(-0.4 * np.abs(np.random.random(N * 800)).reshape(N, -1), axis=1)

        for i in range(N):
            x[i, :] += a[i, 1]

        xi = np.max(x, axis=0)
        ai = np.min(x, axis=0)
        di = np.abs(np.max(d, axis=0))

        if np.any(xi > ai[-1]):
            x = x[: np.argmax(xi > ai[-1])]

        xi = np.min(x, axis=0)

        if np.any(di > xi[-1]):
            d = d[: np.argmax(di > xi[-1])]

        for i in range(N):
            d[i, :] += x[i, -1]

        x = np.hstack((x, d))

        # initialise random generators
        generators.restore(state)
        ilocal = np.zeros(N, dtype=np.int64)
        istart = np.zeros(N, dtype=np.int64)
        istate = np.zeros(N, dtype=np.int64)

        da = generators.random([n + nbuffer])
        istate += n + nbuffer
        state = generators.state()
        chunk = np.cumsum(da, 1) - offset
        nchunk = chunk.shape[-1]

        for t in range(x.shape[1]):

            ilocal = QPot.lower_bound(chunk, x[:, t], ilocal)
            shift = ilocal  # some choice just for testing

            advance = np.where(shift < 0, shift + istart - istate, nchunk + istart - istate)
            generators.advance(advance)
            istate += advance
            n = np.max(np.abs(shift)) + 1
            da = generators.random([n])
            state = generators.state()
            istart += shift
            istate += n

            chunk = QPot.cumsum_chunk(chunk, da, shift)

            for p in range(N):
                u = istart[p]
                v = istart[p] + chunk.shape[-1]
                self.assertTrue(np.all(np.equal(chunk[p, :], a[p, u:v])))


if __name__ == "__main__":

    unittest.main(verbosity=2)
