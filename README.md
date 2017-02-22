Sampled-MinHashing
==================

MinHash-based clustering for binary and weighted dyadic data (e.g. bag of words).

## Installation

Sampled-MinHashing has the following dependencies:

> * swig
> * python
> * python-dev
> * cmake

To install the requirements in Ubuntu do:
~~~~
sudo apt-get install cmake python swig libpyton-dev
~~~~

Then
~~~~
git clone https://github.com/gibranfp/Sampled-MinHashing.git
cd Sampled-MinHashing
mkdir build
cd build
cmake ..
make
sudo make install
~~~~
