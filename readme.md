# ColAndreas Component for open.mp

[![Build and Release](https://github.com/Knogle/ColAndreas/actions/workflows/ci.yml/badge.svg)](https://github.com/Knogle/ColAndreas/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/Knogle/ColAndreas?display_name=tag)](https://github.com/Knogle/ColAndreas/releases)
[![Total Downloads](https://img.shields.io/github/downloads/Knogle/ColAndreas/total.svg)](https://github.com/Knogle/ColAndreas/releases)
[![Latest Release Downloads](https://img.shields.io/github/downloads/Knogle/ColAndreas/latest/total.svg)](https://github.com/Knogle/ColAndreas/releases)

ColAndreas is an open.mp component that provides server-side collision and world queries for San Andreas using the Bullet Physics library.

Current version: **v1.5.0**

Current maintainer: **[Knogle](https://github.com/Knogle)**

## Highlights

- open.mp component builds for Linux and Windows (`ColAndreas.so` / `ColAndreas.dll`).
- Native API for ray casts, contact tests, object collision management and utility conversions.
- Include file and component shipped together for straightforward deployment.
- CI with 32-bit Linux and Windows builds plus native API exposure verification.

## Download and Installation

1. Download the latest archive from the [Releases page](https://github.com/Knogle/ColAndreas/releases).
2. Extract the release into your open.mp server root while keeping folder structure.
3. Ensure at least these files are placed correctly:
   - `components/ColAndreas.so` (Linux)
   - `components/ColAndreas.dll` (Windows)
   - `pawno/include/colandreas.inc`
4. Place the collision database file as `scriptfiles/ColAndreas/ColAndreas.cadb`.
5. Make sure your server configuration loads the component.

## Linux Runtime Note

If you get a missing Bullet shared library error at runtime, your host is missing required Bullet runtime libraries.

You can either:

- Use a static build (`ColAndreas_static.so`) when available, or
- Install Bullet runtime packages from your distribution.

## Build from Source

### Repository Setup

```bash
git clone https://github.com/Knogle/ColAndreas.git
cd ColAndreas
git submodule update --init --recursive
```

### Linux x86 (CMake)

Dependencies (Debian/Ubuntu):

```bash
sudo apt-get update
sudo apt-get install -y cmake ninja-build g++-multilib git
```

Build:

```bash
git clone --depth 1 https://github.com/bulletphysics/bullet3 deps/bullet3
cmake -S deps/bullet3 -B deps/bullet3-build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/deps/bullet3-install" \
  -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" -DCMAKE_SHARED_LINKER_FLAGS="-m32" \
  -DBUILD_SHARED_LIBS=ON -DBUILD_BULLET2_DEMOS=OFF -DBUILD_CPU_DEMOS=OFF -DBUILD_EXTRAS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF
cmake --build deps/bullet3-build --target install --parallel

cmake -S . -B build -G Ninja -DFORCE_32_BIT=ON -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PWD/deps/bullet3-install"
cmake --build build --parallel
```

Output:

- `build/ColAndreas.so` (or `build/ColAndreas_static.so` if linked against static Bullet libraries)

## CI/CD

Workflow file: `.github/workflows/ci.yml`

Triggers:

- `push`
- `pull_request`
- `release` (`published`)
- `workflow_dispatch`

CI checks:

- Configure and build ColAndreas as 32-bit on Linux (Ubuntu 24.04) and Windows (Win32).
- Validate that natives declared in `Server/include/colandreas.inc` match natives registered in `src/ColAndreas.cpp`.

Release behavior:

- Build Linux and Windows artifacts.
- Package `ColAndreas-linux.tar.gz` and `ColAndreas-windows.zip`.
- Upload release package asset automatically to the GitHub release.

## Compatibility

- Runtime target: open.mp servers
- SDK: open.mp SDK (`libs/omp-sdk` submodule)
- Physics engine: Bullet

## Credits

Original project and foundation by **[uL]Pottus**, **[uL]Chris42O** and **[uL]Slice**.
