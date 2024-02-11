# Actual Web Rendering in Terminal

Or just `awrit`.

https://github.com/chase/awrit/assets/5411/2c3b0b02-3c49-443d-9865-95aeeb9d1275

Yep, actual Chromium being rendered in your favorite terminal that supports the [Kitty terminal graphics protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).

**`awrit` works best in [Kitty v0.28 or newer](https://github.com/kovidgoyal/kitty/releases)**

## Why?

- Display documentation from [DevDocs](https://devdocs.io)
- Watch the changes from [Vite](https://vitejs.dev) come to life
- Tiled layout without a tiling window manager using [Kitty's layouts](https://sw.kovidgoyal.net/kitty/layouts/)
- Add fancy UI using web technologies, so NeoVim can pretend it is Emacs instead of the other way around

## Installing from Brew (macOS only)
```bash
brew install chase/tap/awrit
```

## Building

**Prerequisites**

- CMake v3.22 or newer
- [Ninja](https://github.com/ninja-build/ninja/releases)

**Linux**:

- Install these packages: `build-essential ninja-build cmake`

**macOS**:

- Xcode 14.3, try [xcodes](https://github.com/XcodesOrg/xcodes)
- Brew: `brew install ninja cmake`
- MacPorts: `port install ninja cmake`

---

```bash
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build

# The resulting build will be located in build/awrit/Release
# The build can be run from the top-level directory with ./build/awrit/Release/awrit
```

## Installing from Source

After building:

```bash
# install to the prefix ~/.local
cmake --install build --prefix ~/.local

# or if you're feeling adventurous 
# install to the prefix /usr/local
# sudo cmake --install build --prefix /usr/local
```

If the installation prefix is in your `PATH`, it can be run with `awrit`.  
You can check if `awrit` is in your `PATH` with `which awrit`.

## Usage

```bash
awrit [url]

# if url is not provided, it will go to the awrit homepage (this is temporary, promise)
# the URL protocol can be http:, https:, or data:
# if the URL protocol is not included, https: is used by default
```

The `data:` URL in the demo video is the following:

```bash
awrit 'data:text/html;base64,PCFET0NUWVBFIGh0bWw+PGh0bWw+PGhlYWQ+PHN0eWxlPjpyb2
90ey0tYmFja2dyb3VuZDojMjMyYTJlOy0tYmFzZTojRDNDNmFhOy0tYWNjZW50OiM4M2MwOTI7LS1za
GFkb3c6IzRGNUI1OH1odG1se2Rpc3BsYXk6Z3JpZDtoZWlnaHQ6MTAwJTtwbGFjZS1pdGVtczpjZW50
ZXJ9LmNvb2x7Zm9udDpib2xkIDNyZW0vMS4yIHNhbnMtc2VyaWY7bWF4LXdpZHRoOjEycmVtfS5jb29
sIHNwYW57Y29sb3I6dmFyKC0tc2hhZG93KTtkaXNwbGF5OmlubGluZS1ibG9jaztwb3NpdGlvbjpyZW
xhdGl2ZX0uY29vbCBzcGFuOm50aC1jaGlsZCgxKTo6YWZ0ZXIsLmNvb2wgc3BhbjpudGgtY2hpbGQoM
Sk6OmJlZm9yZXthbmltYXRpb24tZGVsYXk6LjI1c30uY29vbCBzcGFuOm50aC1jaGlsZCgyKTo6YWZ0
ZXIsLmNvb2wgc3BhbjpudGgtY2hpbGQoMik6OmJlZm9yZXthbmltYXRpb24tZGVsYXk6LjVzfS5jb29
sIHNwYW46bnRoLWNoaWxkKDMpOjphZnRlciwuY29vbCBzcGFuOm50aC1jaGlsZCgzKTo6YmVmb3Jle2
FuaW1hdGlvbi1kZWxheTouNzVzfS5jb29sIHNwYW46bnRoLWNoaWxkKDQpOjphZnRlciwuY29vbCBzc
GFuOm50aC1jaGlsZCg0KTo6YmVmb3Jle2FuaW1hdGlvbi1kZWxheToxc30uY29vbCBzcGFuOjpiZWZv
cmV7YW5pbWF0aW9uOm1heC1oZWlnaHQgLjRzIGN1YmljLWJlemllciguNjEsMSwuODgsMSkgMSBub3J
tYWwgYm90aDtjb2xvcjp2YXIoLS1hY2NlbnQpfS5jb29sIHNwYW46OmFmdGVye2FuaW1hdGlvbjptYX
gtd2lkdGggLjdzIGN1YmljLWJlemllciguNjEsMSwuODgsMSkgMSBub3JtYWwgYm90aDtjb2xvcjp2Y
XIoLS1iYXNlKX0uY29vbCBzcGFuOjphZnRlciwuY29vbCBzcGFuOjpiZWZvcmV7Y29udGVudDphdHRy
KGRhdGEtdGV4dCk7bGVmdDowO292ZXJmbG93OmhpZGRlbjtwb3NpdGlvbjphYnNvbHV0ZTtzcGVhazp
ub25lfUBrZXlmcmFtZXMgbWF4LXdpZHRoe2Zyb217bWF4LXdpZHRoOjB9dG97bWF4LXdpZHRoOjEwMC
V9fUBrZXlmcmFtZXMgbWF4LWhlaWdodHtmcm9te21heC1oZWlnaHQ6MH10b3ttYXgtaGVpZ2h0OjEwM
CV9fTwvc3R5bGU+PC9oZWFkPjxib2R5PjxwIGNsYXNzPSJjb29sIj48c3BhbiBkYXRhLXRleHQ9IkFX
UklULCI+QVdSSVQsPC9zcGFuPgo8c3BhbiBkYXRhLXRleHQ9IlRIQVQnUyI+VEhBVCdTPC9zcGFuPgo
8c3BhbiBkYXRhLXRleHQ9IlBSRVRUWSI+UFJFVFRZPC9zcGFuPgo8c3BhbiBkYXRhLXRleHQ9IkNPT0
wiPkNPT0w8L3NwYW4+CjwvcD48L2JvZHk+PC9odG1sPg=='
```
