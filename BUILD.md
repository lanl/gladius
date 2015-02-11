# Building Gladius

## Prerequisites
- C++11-capable C++ compiler
- libelf (http://directory.fsf.org/wiki/Libelf)
- libiberty (https://gcc.gnu.org/onlinedocs/libiberty/)
- libmigdb (http://libmigdb.sourceforge.net/)
- libedit (http://thrysoee.dk/editline/)
- LaunchMON (https://github.com/scalability-llnl/LaunchMON)
    - libgcrypt (http://www.gnu.org/software/libgcrypt/)
    - MPI Library (Open MPI or MPICH)
- MRNet (http://www.paradyn.org/mrnet/)
    - boost (http://www.boost.org/)
    - flex (http://flex.sourceforge.net/)
- GNU Autotools (m4, autoconf, automake, libtool)

## Building libmigdb (see: http://libmigdb.sourceforge.net/)
```bash
export MIGDB_PREFIX=$HOME/local/migdb
git clone git@github.com:samuelkgutierrez/libmigdb.git
cd libmigdb
make install prefix=$MIGDB_PREFIX
```

## Building LaunchMON
```bash
export LMON_PREFIX=$HOME/local/lmon
git clone git@github.com:scalability-llnl/LaunchMON.git
cd LaunchMON
./bootstrap
./configure --prefix=$LMON_PREFIX \
MPICC=mpicc MPICXX=mpic++ && \
MAKEINFOFLAGS="--force" make && make install
```
### NOTES:
See: https://forum.videolan.org/viewtopic.php?f=13&t=33860#p121003

Basically, the problem is a missing '-lgpg-error' in testName_LDADD.
testName List
- basic
- tsexp
- keygen
- benchmark

## Building MRNet
Download source distribution from http://www.paradyn.org/mrnet
```bash
export MRNET_PREFIX=$HOME/local/mrnet
./configure CXX=g++ --prefix=$MRNET_PREFIX
cd build/<ARCH>
make -j4 && make install
```

## Building Gladius
```bash
# If NOT building from a distribution, autogen first.
./autogen
```
```bash
./configure \
--prefix=$HOME/local/gladius \
--with-migdb=$MIGDB_PREFIX \
--with-lmon=$LMON_PREFIX \
--with-mrnet=$MRNET_PREFIX
```
