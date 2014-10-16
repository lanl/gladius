# Building Gladius

## Prerequisites
- C++11-capable C++ compiler
- swig (http://www.swig.org/)
- libedit (http://thrysoee.dk/editline/)
- LLDB (http://lldb.llvm.org/)
- MRNet (http://www.paradyn.org/mrnet/)

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

## Building Gladius
```bash
mkdir build && cd build
cmake ../ -DGLADIUS_LLDB_HOME=$LLVM_PREFIX
make
```
