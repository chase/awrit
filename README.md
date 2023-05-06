# Actual Web Rendering in Terminal

Or just AWRIT.

Yep, actual Chromium being rendered in your favorite terminal that supports the [Kitty terminal graphics protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).

## Why?

- Display documentation from [DevDocs](https://devdocs.io)
- Watch the changes from [Vite](https://vitejs.dev) come to life
- Tiled layout without a tiling window manager using [Kitty's layouts](https://sw.kovidgoyal.net/kitty/layouts/)
- Add fancy UI using web technologies, so NeoVim can pretend it is Emacs instead of the other way around

## Building

** Prerequisites **

- CMake v3.22 or newer
- Ninja

- Linux:
    - `build-essential`
- macOS:
    - Xcode 13.4
- Windows 10:
    - Visual Studio 2019 command line tools

---

``` bash
mkdir build && cd build

# On Windows only:
# "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
ninja
```
