## Building

### Dependencies

On Debian:

    sudo apt install build-essential cmake qt6-base-dev

This project uses [vcpkg](https://github.com/microsoft/vcpkg) for C++ dependencies.

Set `VCPKG_ROOT` to your vcpkg installation:

    export VCPKG_ROOT=/path/to/vcpkg

### Build

    cmake --preset release
    cmake --build --preset release
