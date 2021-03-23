import unittest
import numpy as np
import h5py
import time
import QPot

class Test_main(unittest.TestCase):

    def test_Basic(self):

        x = np.array([-1.0, 0.0, 1.0])
        l = np.array([-1.5, -0.5, 0.5])
        r = l + 1.0
        i = np.array([13, 14, 15])

        def uniform(shape):
            return np.ones(shape)

        y = QPot.RedrawList(x, uniform, 30, 5, 2);

        for j in range(20):
            x += 1.0
            l += 1.0
            r += 1.0
            i += 1
            y.setPosition(x)
            self.assertTrue(np.all(np.equal(y.currentYieldLeft(), l)))
            self.assertTrue(np.all(np.equal(y.currentYieldRight(), r)))
            self.assertTrue(np.all(np.equal(y.currentIndex(), i)))

        for j in range(40):
            x -= 1.0
            l -= 1.0
            r -= 1.0
            i -= 1
            y.setPosition(x)
            self.assertTrue(np.all(np.equal(y.currentYieldLeft(), l)))
            self.assertTrue(np.all(np.equal(y.currentYieldRight(), r)))
            self.assertTrue(np.all(np.equal(y.currentIndex(), i)))

    def test_Reconstruct_positions(self):

        seed = int(time.time())
        rand = QPot.random.RandList()

        x = np.array([4.5, 5.5, 6.5])

        QPot.random.seed(seed)
        this = QPot.RedrawList(x, rand, 30, 5, 2)
        n = 50
        redraw = np.zeros((n), dtype=np.int64)

        for i in range(n):
            redraw[i] = this.setPosition(i * x)

        pos = this.raw_pos()
        yl = this.currentYieldLeft()
        yr = this.currentYieldRight()
        yi = this.currentIndex()

        QPot.random.seed(seed)
        other = QPot.RedrawList(x, rand, 30, 5, 2)

        for i in range(n):
            if redraw[i]:
                other.setPosition(i * x)

        other.setPosition(float(n - 1) * x)

        self.assertTrue(np.allclose(pos, other.raw_pos()))
        self.assertTrue(np.allclose(yl, other.currentYieldLeft()))
        self.assertTrue(np.allclose(yr, other.currentYieldRight()))
        self.assertTrue(np.all(np.equal(yi, other.currentIndex())))

    def test_Reconstruct_last_redraw(self):

        seed = int(time.time())
        rand = QPot.random.RandList()

        x = np.array([4.5, 5.5, 6.5])

        QPot.random.seed(seed)
        this = QPot.RedrawList(x, rand, 30, 5, 2)
        n = 50
        redraw = np.zeros((x.size, n), dtype=np.int64)

        for i in range(n):
            this.setPosition(i * x)
            redraw[:, i] = this.last_redraw()

        pos = this.raw_pos()
        yl = this.currentYieldLeft()
        yr = this.currentYieldRight()
        yi = this.currentIndex()

        QPot.random.seed(seed)
        other = QPot.RedrawList(x, rand, 30, 5, 2)

        for i in range(n):
            other.redraw(redraw[:, i])

        other.setPosition(float(n - 1) * x)

        self.assertTrue(np.allclose(pos, other.raw_pos()))
        self.assertTrue(np.allclose(yl, other.currentYieldLeft()))
        self.assertTrue(np.allclose(yr, other.currentYieldRight()))
        self.assertTrue(np.all(np.equal(yi, other.currentIndex())))

    def test_Reconstruct_compact_storage(self):

        seed = int(time.time())
        rand = QPot.random.RandList()

        x = np.array([4.5, 5.5, 6.5])

        QPot.random.seed(seed)
        this = QPot.RedrawList(x, rand, 30, 5, 2)
        n = 50
        index = []
        direction = []

        for i in range(n):
            this.setPosition(i * x)
            iredraw = this.last_redraw()
            if np.any(iredraw > 0):
                index += [np.argwhere(iredraw > 0).ravel()]
                direction += [+1]
            if np.any(iredraw < 0):
                index += [np.argwhere(iredraw < 0).ravel()]
                direction += [-1]

        pos = this.raw_pos()
        yl = this.currentYieldLeft()
        yr = this.currentYieldRight()
        yi = this.currentIndex()

        QPot.random.seed(seed)
        other = QPot.RedrawList(x, rand, 30, 5, 2)

        for i, d in zip(index, direction):
            if d > 0:
                other.redrawRight(i)
            else:
                other.redrawLeft(i)

        other.setPosition(float(n - 1) * x)

        self.assertTrue(np.allclose(pos, other.raw_pos()))
        self.assertTrue(np.allclose(yl, other.currentYieldLeft()))
        self.assertTrue(np.allclose(yr, other.currentYieldRight()))
        self.assertTrue(np.all(np.equal(yi, other.currentIndex())))

    def test_Reconstruct_platform_independence(self):

        with h5py.File("Redraw_reconstruct-data.h5", "r") as data:

            rand = QPot.random.RandList()
            QPot.random.seed(data["/seed"][...])

            n = data["/n"][...]
            redraw = data["/redraw"][...]
            x = data["/x"][...]

            this = QPot.RedrawList(x, rand, data["/ntotal"][...], data["/nbuffer"][...], data["/noffset"][...])

            for i in range(n):
                this.redraw(redraw[:, i])

            this.setPosition(float(n - 1) * x)

            self.assertTrue(np.allclose(this.raw_pos(), data["/raw_pos"][...]))
            self.assertTrue(np.allclose(this.currentYieldLeft(), data["/currentYieldLeft"][...]))
            self.assertTrue(np.allclose(this.currentYieldRight(), data["/currentYieldRight"][...]))
            self.assertTrue(np.all(np.equal(this.currentIndex(), data["/currentIndex"][...])))

if __name__ == '__main__':

    unittest.main()
