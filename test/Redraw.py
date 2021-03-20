import unittest
import numpy as np
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

    def test_Reconstruct(self):

        seed = int(time.time())
        rand = QPot.random.RandList()

        x = np.array([4.5, 5.5, 6.5])

        QPot.random.seed(seed)
        this = QPot.RedrawList(x, rand, 30, 5, 2)
        redraw = np.zeros((50), dtype=np.int64)

        for i in range(redraw.size):
            redraw[i] = this.setPosition(i * x)

        pos = this.raw_pos()

        QPot.random.seed(seed)
        other = QPot.RedrawList(x, rand, 30, 5, 2)

        for i in range(redraw.size):
            if redraw[i]:
                other.setPosition(i * x)

        self.assertTrue(np.allclose(pos, other.raw_pos()))

if __name__ == '__main__':

    unittest.main()
