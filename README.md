# QIDE - The (Q)uake Mod (IDE)

Currently targeting compatibility with the 2021 re-release.

## Building

### Getting the source

```bash
git clone 
```

### Dependencies

- C++20 compiler (GCC 11+)
- CMake
- Qt5 Widgets

#### Ubuntu 20.04+

First it is recommended you enable the [toolchain test builds ppa](https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/test) for the more recent compiler versions.
To enable it run the following commands:

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
```

Then to install all of the build dependencies run the following command:

```bash
sudo apt install g++-11 cmake qtbase5-dev qtbase5-dev-tools
```

### Compiling

From the root directory of the repo run the following commands:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -- -j8
```

Now there should be a folder `build/qide` with the compiled executable inside.

## Running

TODO

