import unittest
import numpy as np
import QPot

class Test_main(unittest.TestCase):

    def test_Basic(self):

        y = QPot.Static(5.5, np.linspace(0, 10, 11))

        self.assertTrue(y.currentIndex() == 5)
        self.assertTrue(y.checkYieldBoundLeft())
        self.assertTrue(y.checkYieldBoundRight())
        self.assertTrue(y.currentYieldLeft() == 5.0)
        self.assertTrue(y.currentYieldRight() == 6.0)

        y.setPosition(6.5)

        self.assertTrue(y.currentIndex() == 6)
        self.assertTrue(y.checkYieldBoundLeft())
        self.assertTrue(y.checkYieldBoundRight())
        self.assertTrue(y.currentYieldLeft() == 6.0)
        self.assertTrue(y.currentYieldRight() == 7.0)

if __name__ == '__main__':

    unittest.main()
