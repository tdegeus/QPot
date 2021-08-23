import os
import re
import sys
import subprocess

from skbuild import setup
from setuptools_scm import get_version

cmake_args = []

if "CMAKE_ARGS" in os.environ:
    if len(os.environ["CMAKE_ARGS"].split(" ")) > 0:
        cmake_args += os.environ["CMAKE_ARGS"].split(" ")

setup(
    name = 'QPot',
    description = 'Library to keep track of a sequential potential energy landscape.',
    long_description = 'Library to keep track of a sequential potential energy landscape.',
    version = get_version(),
    license = 'MIT',
    author = 'Tom de Geus',
    author_email = 'tom@geus.me',
    url = 'https://github.com/tdegeus/QPot',
    cmake_args = cmake_args,
    zip_safe = False,
)
