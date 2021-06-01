from setuptools import setup, find_packages

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir

import sys

__version__ = "0.0.1"

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
        'pyudmaio_binding',
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
        runtime_library_dirs = [
            '/usr/local/lib'
        ]
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
    zip_safe=False,
)
