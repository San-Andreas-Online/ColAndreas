# ColAndreas Component for open.mp

[![Build and Release](https://github.com/Knogle/ColAndreas/actions/workflows/ci.yml/badge.svg)](https://github.com/Knogle/ColAndreas/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/Knogle/ColAndreas?display_name=tag)](https://github.com/Knogle/ColAndreas/releases)
[![Total Downloads](https://img.shields.io/github/downloads/Knogle/ColAndreas/total.svg)](https://github.com/Knogle/ColAndreas/releases)
[![Latest Release Downloads](https://img.shields.io/github/downloads/Knogle/ColAndreas/latest/total.svg)](https://github.com/Knogle/ColAndreas/releases)

ColAndreas is an open.mp component that provides server-side collision and world queries for San Andreas using the Bullet Physics library.

Current version: **v1.5.0**

Current maintainer: **[Knogle](https://github.com/Knogle)**

## Highlights

- open.mp component build (`ColAndreas.so` on Linux).
- Native API for ray casts, contact tests, object collision management and utility conversions.
- Include file and component shipped together for straightforward deployment.
- CI with Linux build and native API exposure verification.

## Download and Installation

1. Download the latest archive from the [Releases page](https://github.com/Knogle/ColAndreas/releases).
2. Extract the release into your open.mp server root while keeping folder structure.
3. Ensure at least these files are placed correctly:
   - `components/ColAndreas.so` (Linux)
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

### Linux (CMake)

Dependencies (Debian/Ubuntu):

```bash
sudo apt-get update
sudo apt-get install -y cmake ninja-build g++ libbullet-dev
```

Build:

```bash
cmake -S . -B build -G Ninja -DFORCE_32_BIT=OFF -DCMAKE_BUILD_TYPE=Release
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

- Configure and build ColAndreas on Ubuntu 22.04.
- Validate that natives declared in `Server/include/colandreas.inc` match natives registered in `src/ColAndreas.cpp`.

Release behavior:

- Build Linux artifact.
- Package `ColAndreas-linux.tar.gz`.
- Upload release package asset automatically to the GitHub release.

## Compatibility

- Runtime target: open.mp servers
- SDK: open.mp SDK (`libs/omp-sdk` submodule)
- Physics engine: Bullet

## Credits

Original project and foundation by **[uL]Pottus**, **[uL]Chris42O** and **[uL]Slice**.
