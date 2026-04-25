I no longer have time to maintain my hobby projects and with the rising number of security issues, I would advise you to look at a more actively maintained fork.

If you use macOS you might enjoy using [cmux](https://github.com/manaflow-ai/cmux) as a replacement for awrit.

---

# Actual Web Rendering in Terminal

Or just `awrit`.

[awrit-demo.webm](https://github.com/user-attachments/assets/5da3fffc-d781-4b00-9fe3-19ce18d01a7e)

Yep, actual Chromium being rendered in your favorite terminal that supports the [Kitty terminal graphics protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).

**`awrit` works best in [Kitty v0.31 or newer](https://github.com/kovidgoyal/kitty/releases)**

## Why?

- Display documentation from [DevDocs](https://devdocs.io)
- Watch the changes from [Vite](https://vitejs.dev) come to life
- Tiled layout without a tiling window manager using [Kitty's layouts](https://sw.kovidgoyal.net/kitty/layouts/)
- Add fancy UI using web technologies, so NeoVim can pretend it is Emacs instead of the other way around

## Usage

```bash
awrit [url]

# if url is not provided, it will go to the awrit homepage (this is temporary, promise)
# the URL protocol can be http:, https:, or data:
# if the URL protocol is not included, https: is used by default
```

For more options look at the help:

```bash
awrit --help
```

## Configuration

`awrit` can be configured through `config.js` in the project root. Changes to it will update the config in any running `awrit`.

Currently it only supports custom keybindings and changing the homepage that displays when no URL is provided.

For more details on keybinding syntax and available actions, see the comments in `config.js`.

## Contributing

See [Contributing to Awrit](/CONTRIBUTING.md#contributing-to-awrit).

## Development

Assuming you already have `git` installed, your installation of `awrit` will already be a Git repository.

You can update `awrit` to use your fork by changing the origin:

``` bash
# note: you'll have to change the username some-kind-contributor to your GitHub username
git remote set-url origin git@github.com:some-kind-contributor/awrit.git
# also track the upstream electron branch
git remote add upstream -f -t electron git@github.com:chase/awrit.git
```

You can make a branch (ex: my-feature-branch) off the latest changes by doing:

```
git fetch upstream electron
git checkout upstream/electron
git switch -c 'my-feature-branch'
```

Read [Your First Code Contribution](/CONTRIBUTING.md#your-first-code-contribution) for more information on making a PR.
