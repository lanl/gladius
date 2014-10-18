# Building Gladius

## Prerequisites
- C++11-capable C++ compiler
- LLDB (http://lldb.llvm.org/)
    - swig (http://www.swig.org/)
    - libedit (http://thrysoee.dk/editline/)
- MRNet (http://www.paradyn.org/mrnet/)
    - boost (http://www.boost.org/)
    - flex (http://flex.sourceforge.net/)

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
mkdir build && cd build
cmake ../ \
-DGLADIUS_LLDB_HOME=$LLVM_PREFIX
-DGLADIUS_MRNET_HOME=$MRNET_PREFIX
make
```
