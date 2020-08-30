# IXO
Object serialization and deserialization library: JSON, Binary formats

## Installation for developers
To install the library to be accessible to other projects,
run `python3 install.py`

This scripts asks to auto detect the `lib/` and `include/` paths by looking
at the location of `gcc` (this should work with GCC and Clang). If you use
another compiler, you can press `n` and insert the corresponding paths
individually.

## Linking
Programs that use IXO should link to `libIXO.a`.

## Documentation
[Here](doc/main.md)
