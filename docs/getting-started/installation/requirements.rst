.. _installation_requirements:

Requirements
============
 - `CMake >= 3.16 <https://cmake.org/>`
 - `Conan >= 1.41 <https://conan.io/>`

When only using the python bindings the build process of the package will download the requirements automatically.
However, the usage of PER as c++ library requires the user to download these themselves and make them available
to be found by CMake.

Conan is setup to start upon configuring cmake to download the
following C++ library dependencies from the Conan repositories (see 'deps/conanfile.txt'):

 - `GTest`
 - `pybind11`
 - `cppitertools`
