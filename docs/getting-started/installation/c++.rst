.. _installation_cpp:

C++ Library
===========

In order to install PER, download the requirements to your system first.
Then download the repository to a directory of your choice e.g. via::

   git clone https://github.com/maichmueller/per.git

and step inside the cloned repo ``cd per``.
From inside the folder call::

    cmake -S . -B ./build -DENABLE_BUILD_PYTHON_EXTENSION=OFF
    cmake --build ./build --config Release --target tests
    cmake --install ./build --prefix=MY_INSTALLATION_FOLDER


