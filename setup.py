#! usr/bin/env python

from distutils.core import setup
import pyplus1


setup(name='pyplus1',
      version='1.0',
      py_modules=["pyplus1"],
      ext_modules=[pyplus1.ffi.verifier.get_extension()])

