# libflow (v0.1)

## What's that?

libflow is a simple yet robust data-flow pipeline library written using C++14.

- following SYR principle, libflow can be (re)used almost whenever everywhere

## Compiling

libflow currently supports Linux or any Unix-like system. There is no official support for Windows/Mac (yet). However, strict coding rules and C++14 standard conformance, should also allow for flawless compilation under Windows or Mac. That applies to using rather recent C++14 compiler (gcc, clang or msvc).

```bash
git clone https://github.com/barczynsky/libflow.git
cd libflow
mkdir build && cd build
cmake ..
make
```

You will end up with the main libflow shared object (libflow.so) and a simple dataflow example, usually presenting recent features and fixes.

- Recommended C++ standard used for compiling is C++14. Only some minor elements of it may be in use, but this alone needs a C++14 conformant compiler or high enough standard extensions support.

## Dependencies

- C++ Standard Library
- tea and chocolate
- a chair
- You

## Features

- non-template common base class (Node)
- template derived classes thus accepting any type
- input form std::istream into DataBlock objects (INode)
- output to std::ostream from DataBlock objects (ONode)
- dedicated node well suited for I/O buffering (IONode)

## TODO

- think over ConvertNode class (storage-less converter)
- create other nodes (e.g. ZeroMQ sockets)
- consider using weak_ptr for storing node references
