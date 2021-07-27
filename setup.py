from __future__ import print_function

import sys

try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n"
        " or you need to install the PEP 518 requirements in pyproject.toml yourself",
        file=sys.stderr,
    )
    raise

from setuptools import find_packages

setup(
    name="pyper",
    version="0.0.1",
    author="Michael Aichmueller",
    author_email="m.aichmueller@gmail.com",
    description="An implementation of Prioritized Experience Replay",
    long_description="",
    license="MIT",
    packages=find_packages(where='src'),
    package_dir={"": "src"},
    cmake_install_dir="src/pyper",
    include_package_data=True,
)
