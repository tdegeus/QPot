from skbuild import setup
from setuptools_scm import get_version

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
    cmake_minimum_required_version = "3.13...3.21",
)
