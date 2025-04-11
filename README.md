# refl-cpp [WIP]
A simple reflection library for `C++`, with a hands-off approach.
Only one macro to mark the `class/struct` to be reflected.

# Getting Started
The actual reflection library is a header-only library. Since there are
implementation header so to say. You should always `include<refl-cpp/refl-cpp.hpp>`.
It is also marked as precompile-header to further speed up the build process.

## TODO
- [critical] add code generator
- [critical] add easy build hook for projects
- support template parameters?
- add reflection for enums

## Contribute
The project is not in a state where contribution is easily possible.
If necessity comes I will work on that just open a pull request saying
want to contribute.

## Dependencies
these are getting managed through [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

- [ccache.cmake](https://github.com/TheLartians/Ccache.cmake)
- [LLVM / Clang](https://github.com/llvm/llvm-project)
- [googletest](https://github.com/google/googletest)
- [fmt](https://github.com/fmtlib/fmt)
