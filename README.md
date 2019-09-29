# Attempt to run Objective-C and Swift code with LLVM JIT

This is a highly experimental proof of concept which was supposed to serve as
an Objective-C/Swift backend for Mull mutation testing tool. We have never
finished this project because we mostly focused on C/C++ since then.

See these articles for a background:

- [LLVM JIT, Objective-C and Swift on macOS: knowledge dump](https://stanislaw.github.io/2018/09/03/llvm-jit-objc-and-swift-knowledge-dump.html)
- [Mutation testing for Swift with Mull: how it could work. Looking for contributors.](https://stanislaw.github.io/2018/09/03/mull-and-swift-how-it-almost-works.html)

## Setup

### Preconditions

You need to have a binary version of LLVM 6 installed on your machine in
`/opt/llvm-6.0.0` folder.

The binary distribution of LLVM 6 can be found on
[LLVM Download Page](http://releases.llvm.org/download.html) page.

### Creating a project

The project works on macOS only.

To run with Xcode, clone the repository and run:

```
make xcode.build
make xcode.open
```

Otherwise the project is a normal CMake project so opening it with any
CMake-aware IDE like CLion should work out of the box.
