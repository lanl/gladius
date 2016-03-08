# Building Gladius

## Prerequisites
- C++11-capable C++ compiler
- libedit (http://thrysoee.dk/editline/)
- MPI Library (Open MPI or MPICH)
- MRNet (http://www.paradyn.org/mrnet/)
    - boost (http://www.boost.org/)
    - flex (http://flex.sourceforge.net/)
- GNU Autotools (m4, autoconf, automake, libtool)

## Building MRNet
Download source distribution from http://www.paradyn.org/mrnet

```
export MRNET_PREFIX=$HOME/local/mrnet
./configure CXX=g++ --prefix=$MRNET_PREFIX
cd build/<ARCH>
make -j4 && make install
```

## Building Gladius
```
# If NOT building from a distribution, autogen first.
./autogen
```

```
./configure \
MPICXX=[MPI C++ Wrapper Compiler] \
--prefix=$HOME/local/gladius \
--with-mrnet=$MRNET_PREFIX
```

```
make && make install
```
