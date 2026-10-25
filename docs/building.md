# Building femtoCAD

## Dependencies

On Debian:

    sudo apt install build-essential cmake qt6-base-dev

This project uses [vcpkg](https://github.com/microsoft/vcpkg) for C++ dependencies.

Set `VCPKG_ROOT` to your vcpkg installation:

    export VCPKG_ROOT=/path/to/vcpkg

## Build

    cmake --preset release          # or: debug, when you need debug info
    cmake --build --preset release

Binaries land in `build/<preset>/`. The app is `build/<preset>/src/app/femtoCAD`.

## Test

    ctest --preset release

The tests of each library are `build/<preset>/src/libs/<library>/tests/femtoCAD-<library>-tests`.
