<p><br></p>

<p align="center"><img src="qide/img/icon-256.png" alt="Icon"></p>

<p><br></p>

<h1 align="center">
  QIDE - The (Q)uake Mod (IDE)<p><br><p>
  <a href="https://github.com/RamblingMadMan/qide#About">About</a> - 
  <a href="https://github.com/RamblingMadMan/qide#Building">Building</a> - 
  <a href="https://github.com/RamblingMadMan/qide#Running">Running</a> - 
  <a href="https://github.com/RamblingMadMan/qide#Screenshots">Screenshots</a>
  <p><br></p>
</h1>

<p><br></p>

# About

Currently targeting compatibility with the 2021 re-release.

<p><br></p>

# Building

The build process is pretty similar to other CMake based projects, and should be fairly straightforward.

<p><br></p>

### Getting the source

```bash
git clone --depth 1 https://github.com/RamblingMadMan/qide.git
```

<p><br></p>

### Dependencies

- C++20 compiler (GCC 11+)
- CMake
- Qt5 Widgets

<p><br></p>

#### Ubuntu 20.04+

First it is recommended you enable the [toolchain test builds ppa](https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/test) for the more recent compiler versions.
To enable it run the following commands:

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
```

Then to install all of the build dependencies run the following command:

```bash
sudo apt install g++-11 cmake qtbase5-dev qtbase5-dev-tools libzip-dev
```

<p><br></p>

### Compiling

From the root directory of the repo run the following commands:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -- -j8
```

Now there should be a folder `build/qide` with the compiled executable inside.

<p><br></p>

# Running

TODO

<p><br></p>

# Screenshots

<p align="center"><img src="res/screenshot-wizard.png" width="1024" alt="Screenshot of wizard"></p>
<p align="center"><img src="res/screenshot.png" width="1024" alt="Screenshot of main interface"></p>

