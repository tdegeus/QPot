import os
import re
import sys
import subprocess

from skbuild import setup
from setuptools_scm import get_version

cmake_args = []

if "CMAKE_ARGS" in os.environ:
    args = list(filter(None, os.environ["CMAKE_ARGS"].split(" ")))
    if len(args) > 0:
        cmake_args += [arg for arg in args if len(arg.split("DCMAKE_INSTALL_PREFIX")) == 1]

setup(
    name = 'QPot',
    description = 'Library to keep track of a sequential potential energy landscape.',
    long_description = 'Library to keep track of a sequential potential energy landscape.',
    version = get_version(),
    license = 'MIT',
    author = 'Tom de Geus',
    author_email = 'tom@geus.me',
    url = 'https://github.com/tdegeus/QPot',
    packages = ['QPot'],
    package_dir = {'': 'python'},
    cmake_install_dir = 'python/QPot',
    cmake_args = cmake_args,
    cmake_minimum_required_version = "3.13...3.21",
)
