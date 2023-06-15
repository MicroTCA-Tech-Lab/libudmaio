# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

from setuptools import setup, find_packages

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir

import os
import re
import glob

# Use same version for python package as for the whole library.
# Assumes CMakeLists.txt is one dir above this script and contains "project(libudmaio VERSION x.y.z)"
def get_version_from_cmakelists():
    re_ver = re.compile(r'^project\(libudmaio\s+VERSION\s+(\d+\.\d+\.\d+)\)')
    script_path = os.path.dirname(os.path.realpath(__file__))
    print(os.getcwd(), glob.glob("*"))
    with open(os.path.join(script_path, '..', 'CMakeLists.txt'), 'r') as f:
        for l in f:
            m = re_ver.match(l)
            if m:
                return m.group(1)
    
    raise RuntimeError('Version string not found in CMakeLists.txt!')

__version__ = get_version_from_cmakelists()

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension(
        'pyudmaio.binding',
        sources = [
            'src/DataHandlerPython.cpp',
            'src/PythonBinding.cpp'
        ],
        # Example: passing in the version to the compiled code
        define_macros = [
            ('VERSION_INFO', __version__),
            ('BOOST_ALL_DYN_LINK', None),
        ],
        libraries = [
            'udmaio'
        ],
        ),
]

setup(
    name="pyudmaio",
    version=__version__,
    author="Patrick Huesmann, Jan Marjanovic",
    author_email="patrick.huesmann@desy.de",
    url="https://techlab.desy.de/",
    description="Python binding for libudmaio",
    long_description="",
    ext_modules=ext_modules,
    extras_require={
    #    "test": "pytest"
    },
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={
        "build_ext": build_ext
    },
    packages=find_packages(exclude=['tests']),
    package_data={
        package: ["**/*.pyi"] for package in find_packages(exclude=['tests'])
    },
    zip_safe=False,
)
