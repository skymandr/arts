import pyarts.arts as cxx
import test_functions as test

x = cxx.GriddedField4()
test.io(x, delete=True)