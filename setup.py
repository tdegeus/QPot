import re
import os
import pybind11
import pyxtensor
from setuptools import setup, Extension
from setuptools_scm import get_version

version = get_version()

include_dirs = [
    os.path.abspath('include/'),
    pyxtensor.find_pyxtensor(),
    pyxtensor.find_pybind11(),
    pyxtensor.find_xtensor(),
    pyxtensor.find_xtl()]

build = pyxtensor.BuildExt

xsimd = pyxtensor.find_xsimd()

if xsimd:
    if len(xsimd) > 0:
        include_dirs += [xsimd]
        build.c_opts['unix'] += ['-march=native', '-DXTENSOR_USE_XSIMD']
        build.c_opts['msvc'] += ['/DXTENSOR_USE_XSIMD']

build.c_opts['unix'] += ['-DQPOT_VERSION="{0:s}"'.format(version)]
build.c_opts['msvc'] += ['/DQPOT_VERSION="{0:s}"'.format(version)]

ext_modules = [Extension(
    'QPot',
    ['python/main.cpp'],
    include_dirs = include_dirs,
    language = 'c++')]

setup(
    name = 'QPot',
    description = 'Library to keep track of a sequential potential energy landscape.',
    long_description = 'Library to keep track of a sequential potential energy landscape.',
    keywords = 'Yielding; Potential energy landscape',
    version = version,
    license = 'MIT',
    author = 'Tom de Geus',
    author_email = 'tom@geus.me',
    url = 'https://github.com/tdegeus/QPot',
    ext_modules = ext_modules,
    install_requires = ['pybind11', 'pyxtensor'],
    cmdclass = {'build_ext': build},
    zip_safe = False)
