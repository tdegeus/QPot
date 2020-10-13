
import numpy as np
import QPot

y = QPot.Static(5.5, np.linspace(0, 10, 11))

assert y.currentIndex() == 5
assert y.checkYieldBoundLeft()
assert y.checkYieldBoundRight()
assert y.currentYieldLeft() == 5.0
assert y.currentYieldRight() == 6.0

y.setPosition(6.5)

assert y.currentIndex() == 6
assert y.checkYieldBoundLeft()
assert y.checkYieldBoundRight()
assert y.currentYieldLeft() == 6.0
assert y.currentYieldRight() == 7.0
