import os
import re
import sys
import subprocess

from skbuild import setup
from setuptools_scm import get_version

cmake_args = []

if "CMAKE_ARGS" in os.environ:
    o = list(filter(None, os.environ["CMAKE_ARGS"].split(" ")))
    if len(o) > 0:
        cmake_args += o

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
