<p><br></p>

<p align="center"><img src="qide/img/qide.png" alt="Icon"></p>

<p><br></p>

<h1 align="center">
  QIDE - The (Q)uake Mod (IDE)<p><br><p>
  <a href="https://github.com/RamblingMadMan/qide#About">About</a> - 
  <a href="https://github.com/RamblingMadMan/qide#Getting-It">Getting It</a> - 
  <a href="https://github.com/RamblingMadMan/qide#Screenshots">Screenshots</a>
  <p><br></p>
</h1>

<div align="center">

![CI](https://github.com/RamblingMadMan/qide/actions/workflows/ci.yaml/badge.svg)
![AppImage Autobuild](https://github.com/RamblingMadMan/qide/actions/workflows/appimage.yaml/badge.svg)
![MinGW-w64 Autobuild](https://github.com/RamblingMadMan/qide/actions/workflows/cross-mingw-w64.yaml/badge.svg)

</div>

<p><br></p>

# About

QIDE aims to lower the barrier to entry for newcomers to QuakeC and make Quake modding more easily accessable to a wider audience.

The project currently targets compatibility with the 2021 re-release.

<p><br></p>

## Features

| Status | Feature | Description |
| :---: | :----- | :----- |
|  ✅  | Automatic setup | Automatically find Quake (or use built-in shareware) and download [FTEQW](https://fte.triptohell.info/) if needed. |
|  ✅  | Integrated compiler | [Modified version of GMQCC](https://github.com/RamblingMadMan/gmqcc) built-in. |
|  ❓  | Pak Explorer | View, add, remove and modify files within paks. |
|  ✅  | Separate work and data directories | QIDE keeps your project and Quake directories clean. |
|  ✅  | Launch from editor | Launch Quake with your mod enabled, directly from the editor. |
|  ❓  | Syntax/Semantic highlighting | Make your QuakeC source look pretty. |
|  ❓  | Auto-complete | Auto-complete QuakeC expressions. |
|  ❌  | Code search | Search for expressions across multiple files. |
|  ❓  | In-built map editor | Edit maps with pausable live gameplay. |
|  ❌  | QuakeC debugging | Debug your code in real-time. |
|  ❌  | QuakeC VM | Test out simple QuakeC snippets and integrate them with native code. |

<p><br></p>

# Getting It

There are currently no versioned releases of QIDE and it is in active development with many things changing and possibly breaking.

That said, for the keen and able; there are automatically built "autobuild" executables available.

<p><br></p>

## Autobuilds

Automatic builds are available for linux in AppImage format and windows in a self-contained zip.
These builds are a work in progress, but will be stay up-to-date with the latest features.

[Download the latest Autobuild here](https://github.com/RamblingMadMan/qide/releases/tag/autobuild)

<p><br></p>

## Building

The build process is pretty similar to other CMake based projects, and should be fairly straightforward.

<p><br></p>

### Getting the source

```bash
git clone --recursive --depth 1 -j8 https://github.com/RamblingMadMan/qide.git
```

<p><br></p>

### Dependencies

- C++17 compiler
- [CMake 3.16+](https://cmake.org/)
- Qt 5.15 (Widgets + Core)
- OpenSSL

<p><br></p>

#### Ubuntu 20.04+

To install all of the build dependencies, run the following command:

```bash
sudo apt install g++ cmake qtbase5-dev qtbase5-dev-tools libssl-dev
```

<p><br></p>

#### Windows 10

The easiest way to build the project is to install and run [Qt Creator](https://www.qt.io/product/development-tools), open the project and hit build.

It is recommended to use the latest version of MinGW packaged with the Qt online installer, as there are bugs with MSVC and the QuakeC compiler.

> When installing Qt make sure to install Qt 5.15.x and MinGW 8+

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

# Screenshots

| Code editor |
|:-----------:|
| <p align="center"><img src="res/qide1.png" width="1024" alt="Screenshot of main interface"></p> |

| Setup wizard | Project wizard | File wizard |
|:------------:|:--------------:|:-----------:|
| <p align="center"><img src="res/qide2.png" width="512" alt="Screenshot of setup wizard"></p> | <p align="center"><img src="res/qide3.png" width="512" alt="Screenshot of new project wizard"></p> | <p align="center"><img src="res/qide4.png" width="512" alt="Screenshot of new file wizard"></p> |

<p><br></p>

# Special Thanks

Thank you to all of the people involved in all of the projects that made QIDE possible.

Projects used:

- [FTEQW](https://fte.triptohell.info/)
- [GMQCC](https://graphitemaster.github.io/gmqcc/) (using [my fork](https://github.com/RamblingMadMan/gmqcc))
- [glbinding](https://github.com/cginternals/glbinding)
- [rapidfuzz-cpp](https://github.com/maxbachmann/rapidfuzz-cpp)
- [{fmt}](https://github.com/fmtlib/fmt)
- [glm](https://github.com/g-truc/glm)
- [UTF8-CPP](https://github.com/nemtrif/utfcpp)
- [Source Sans font](https://github.com/adobe-fonts/source-sans)
- [Hack font](https://github.com/source-foundry/Hack)
- [Monoid font](https://github.com/larsenwork/monoid)
- [Eva icons](https://github.com/akveo/eva-icons)
