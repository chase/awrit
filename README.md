# Actual Web Rendering in Terminal

Or just `awrit`.

https://github.com/chase/awrit/assets/5411/2c3b0b02-3c49-443d-9865-95aeeb9d1275

Yep, actual Chromium being rendered in your favorite terminal that supports the [Kitty terminal graphics protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).

## Why?

- Display documentation from [DevDocs](https://devdocs.io)
- Watch the changes from [Vite](https://vitejs.dev) come to life
- Tiled layout without a tiling window manager using [Kitty's layouts](https://sw.kovidgoyal.net/kitty/layouts/)
- Add fancy UI using web technologies, so NeoVim can pretend it is Emacs instead of the other way around

## Building

**Prerequisites**

- CMake v3.22 or newer
- [Ninja](https://github.com/ninja-build/ninja/releases)


**Linux**:
  - `build-essential`

**macOS**:
  - Xcode 13.4

**Windows 10**:
  - Visual Studio 2019 command line tools

---

``` bash
# On Windows only:
# "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build

# The resulting build will be located in build/awrit/Release
```
