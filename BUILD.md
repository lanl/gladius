# Building Gladius

## Prerequisites
- C++11-capable C++ compiler
- libelf (http://directory.fsf.org/wiki/Libelf)
- libiberty (https://gcc.gnu.org/onlinedocs/libiberty/)
- LLDB (http://lldb.llvm.org/)
    - swig (http://www.swig.org/)
    - libedit (http://thrysoee.dk/editline/)
- LaunchMON (https://github.com/scalability-llnl/LaunchMON)
    - libgcrypt (http://www.gnu.org/software/libgcrypt/)
    - MPI Library (Open MPI or MPICH)
- MRNet (http://www.paradyn.org/mrnet/)
    - boost (http://www.boost.org/)
    - flex (http://flex.sourceforge.net/)
- GNU Autotools (m4, autoconf, automake, libtool)

## Building LLDB (see: http://lldb.llvm.org/build.html)
```bash
export LLVM_PREFIX=$HOME/local/lldb
svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm
cd llvm/tools
svn co http://llvm.org/svn/llvm-project/cfe/trunk clang
svn co http://llvm.org/svn/llvm-project/lldb/trunk lldb
cd .. && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$LLVM_PREFIX ../
make && make install
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
--with-lmon=$LMON_PREFIX \
--with-mrnet=$MRNET_PREFIX
```
