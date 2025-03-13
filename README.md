# refl-cpp [WIP]
A more simple reflection library for `C++`.
With a hands-off approach.
Only one macro to mark the `class/struct` to be reflected.

## State
- simple basic reflection is working
- no code generation at this point though

## Limitations
- no support for functions with template parameters
- no idea what it looks like for `class/struct` with template parameters (probably not supported)

## TODO
- [critical] add members support
- [critical] add code generator
- [critical] add easy build hook for projects
- support template parameters
- add constructor support

## Dependencies
these are getting managed through [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

- ccache.cmake
- LLVM / Clang
