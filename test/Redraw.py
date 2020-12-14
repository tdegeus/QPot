import unittest
import numpy as np
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

if __name__ == '__main__':

    unittest.main()
