import faulthandler
import unittest

import numpy as np
import QPot

faulthandler.enable()


class Test_find(unittest.TestCase):
    """
    Test QPot.lower_bound*
    """

    def test_find_matrix_random(self):

        a = np.cumsum(0.1 + np.random.random(50000)).reshape(10, -1)
        tests = np.random.randint(0, a.shape[1], 3000).reshape(a.shape[0], -1)
        i = np.zeros(a.shape[0], dtype=np.int64)

        for col in range(tests.shape[1]):
            value = 0.05 + a[np.arange(a.shape[0]), tests[:, col]]
            i = QPot.lower_bound(a, value, i)
            self.assertTrue(np.all(np.equal(tests[:, col], i)))

    def test_out_of_bounds(self):

        a = np.arange(10).astype(np.float64)
        self.assertTrue(QPot.lower_bound(a, -1.0) == 0)
        self.assertTrue(QPot.lower_bound(a, 0.0) == 0)
        self.assertTrue(QPot.lower_bound(a, 0.5) == 0)
        self.assertTrue(QPot.lower_bound(a, 1.5) == 1)
        self.assertTrue(QPot.lower_bound(a, 100) == a.size - 1)

        i = QPot.lower_bound(a, -1.0)
        i = QPot.lower_bound(a, -1.0, i)
        self.assertTrue(i == 0)

        i = QPot.lower_bound(a, 100.0, i)
        self.assertTrue(i == a.size - 1)


if __name__ == "__main__":

    unittest.main(verbosity=2)
